#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Util.h"
#include "Object.h"

using namespace DirectX;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Mesh::sDevice_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!Mesh::sDevice_);

	Mesh::sDevice_ = device;

	// �}�e���A���̐ÓI������
	Material::StaticInitialize(device);
}

Mesh::Mesh()
{
	globalTransform_ = XMMatrixIdentity();

	cbt_.Initialize();
}

void Mesh::SetName(const std::string& name)
{
	name_ = name;
}

void Mesh::AddVertex(const VertexPosNormalUvSkin& vertex)
{
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(uint16_t index)
{
	indices_.emplace_back(index);
}

void Mesh::PopIndex()
{
	indices_.pop_back();
}

void Mesh::AddSmoothData(uint16_t indexPosition, uint16_t indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr) {
		// �e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<uint16_t>& v = itr->second;
		// �S���_�̖@���𕽋ς���
		XMVECTOR normal = {};
		for (uint16_t index : v) {
			normal += XMVectorSet(vertices_[index].normal.x, vertices_[index].normal.y, vertices_[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (uint16_t index : v) {
			vertices_[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}

void Mesh::CalculateTangent()
{
	for (int32_t i = 0; i < indices_.size() / 3; i++)
	{//�O�p�`����ƂɌv�Z
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		uint16_t index0 = indices_[i * 3 + 0];
		uint16_t index1 = indices_[i * 3 + 1];
		uint16_t index2 = indices_[i * 3 + 2];

		// Shortcuts for vertices
		XMFLOAT3& v0 = vertices_[index0].pos;
		XMFLOAT3& v1 = vertices_[index1].pos;
		XMFLOAT3& v2 = vertices_[index2].pos;

		// Shortcuts for UVs
		XMFLOAT2& uv0 = vertices_[index0].uv;
		XMFLOAT2& uv1 = vertices_[index1].uv;
		XMFLOAT2& uv2 = vertices_[index2].uv;

		// Edges of the triangle : postion delta
		XMFLOAT3 deltaPos1 = v1 - v0;
		XMFLOAT3 deltaPos2 = v2 - v0;

		// UV delta
		XMFLOAT2 deltaUV1 = uv1 - uv0;
		XMFLOAT2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		XMFLOAT3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		XMFLOAT3 biTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		Vec3 n = { vertices_[index0].normal.x,vertices_[index0].normal.y,vertices_[index0].normal.z };
		Vec3 t = { tangent.x,tangent.y,tangent.z };
		Vec3 b = { biTangent.x,biTangent.y,biTangent.z };

		// Gram-Schmidt orthogonalize
		t = (t - n * n.Dot(t)).GetNormalized();

		// Calculate handedness
		if (n.Cross(t).Dot(b) < 0.0f) {
			t = t * -1.0f;
		}

		vertices_[index0].tangent = { t.x_,t.y_,t.z_,0 };
		vertices_[index1].tangent = { t.x_,t.y_,t.z_,0 };
		vertices_[index2].tangent = { t.x_,t.y_,t.z_,0 };
	}
}

void Mesh::SetMaterial(Material* material)
{
	material_ = material;
}

void Mesh::CreateBuffers()
{
	HRESULT result = {};

	uint32_t sizeVB = static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices_.size());

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));


	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(VertexPosNormalUvSkin);

	if (indices_.size())
	{
		// ���\�[�X�ݒ�
		resourceDesc.Width = sizeIB;

		// �C���f�b�N�X�o�b�t�@����
		BuffProperties(heapProps, resourceDesc, &indexBuff_);

		// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
		uint16_t* indexMap = nullptr;
		result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
		if (SUCCEEDED(result)) {

			std::copy(indices_.begin(), indices_.end(), indexMap);

			indexBuff_->Unmap(0, nullptr);
		}

		// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.Format = DXGI_FORMAT_R16_UINT;
		ibView_.SizeInBytes = sizeIB;
	}
}

void Mesh::SendingMat(Vec3 materialExtend, const ConstBuffTransform& cbt)
{
	cbt_.SetWorldMat(globalTransform_);
	cbt_.SetViewProjMat(cbt.GetViewProjMat());
	cbt_.SetCameraPos(cbt.GetCameraPos());

	//�}�e���A��
	XMFLOAT3 amb = material_->ambient_;
	material_->ambient_ = material_->ambient_ * materialExtend.x_;
	XMFLOAT3 diff = material_->diffuse_;
	material_->diffuse_ = material_->diffuse_ * materialExtend.y_;
	XMFLOAT3 spe = material_->specular_;
	material_->specular_ = material_->specular_ * materialExtend.z_;
	//�X�V
	material_->Update();

	//�߂�
	material_->ambient_ = amb;
	material_->diffuse_ = diff;
	material_->specular_ = spe;
}

void Mesh::Draw(Vec3 materialExtend, const ConstBuffTransform& cbt,
	const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	SendingMat(materialExtend, cbt);

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = material_->textureHandle_;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(Object::ROOTPARAM_NUM::TEX, srvGpuHandle);

	// �}�e���A���̒萔�o�b�t�@���Z�b�g
	ID3D12Resource* constBuff = material_->GetConstantBuffer();
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(Object::ROOTPARAM_NUM::MATERIAL, constBuff->GetGPUVirtualAddress());

	//���b�V�����Ƃ̍s��
	cbt_.DrawCommand(Object::ROOTPARAM_NUM::MESH_MAT);

	// ���_�o�b�t�@���Z�b�g
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView_);


	// �`��R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices_.size(), 1, 0, 0, 0);
}