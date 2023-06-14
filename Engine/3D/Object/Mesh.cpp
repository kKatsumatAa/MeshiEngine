#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Mesh::device = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!Mesh::device);

	Mesh::device = device;

	// �}�e���A���̐ÓI������
	Material::StaticInitialize(device);
}

void Mesh::SetName(const std::string& name)
{
	this->name = name;
}

void Mesh::AddVertex(const VertexPosNormalUvSkin& vertex)
{
	vertices.emplace_back(vertex);
}

void Mesh::AddIndex(uint16_t index)
{
	indices.emplace_back(index);
}

void Mesh::PopIndex()
{
	indices.pop_back();
}

void Mesh::AddSmoothData(uint16_t indexPosition, uint16_t indexVertex)
{
	smoothData[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr) {
		// �e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<uint16_t>& v = itr->second;
		// �S���_�̖@���𕽋ς���
		XMVECTOR normal = {};
		for (uint16_t index : v) {
			normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (uint16_t index : v) {
			vertices[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}

void Mesh::SetMaterial(Material* material)
{
	this->material = material;
}

void Mesh::CreateBuffers()
{
	HRESULT result = {};

	uint32_t sizeVB = static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices.size());
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));


	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(VertexPosNormalUvSkin);

	// ���\�[�X�ݒ�
	resourceDesc.Width = sizeIB;

	// �C���f�b�N�X�o�b�t�@����
	BuffProperties(heapProps, resourceDesc, &indexBuff);

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {

		std::copy(indices.begin(), indices.end(), indexMap);

		indexBuff->Unmap(0, nullptr);
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// ���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	cmdList->IASetIndexBuffer(&ibView);
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = material->textureHandle;
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// �}�e���A���̒萔�o�b�t�@���Z�b�g
	ID3D12Resource* constBuff = material->GetConstantBuffer();
	cmdList->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());

	// �`��R�}���h
	cmdList->DrawIndexedInstanced((uint32_t)indices.size(), 1, 0, 0, 0);
}
