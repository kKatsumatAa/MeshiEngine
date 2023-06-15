#include "ModelFBX.h"
#include "DirectXWrapper.h"

ModelFBX::~ModelFBX()
{
	//fbx�V�[���̉���iFBXLoader��Finalize()�̑O����Ȃ��ƃG���[�o��j
	fbxScene_->Destroy();
}

void ModelFBX::CreateBuffers()
{
	HRESULT result;//

	//���_�f�[�^�S�̂̃T�C�Y
	uint32_t sizeVB =
		static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	//���_�o�b�t�@����
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//���_�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, vertBuff_.GetAddressOf());

	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//���_�C���f�b�N�X�S�̂̃T�C�Y
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices_.size());
	//����
	ResourceProperties(resDesc, sizeIB);
	BuffProperties(heapProp, resDesc, indexBuff_.GetAddressOf());

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	uint16_t* indexmap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexmap);
	if (SUCCEEDED(result))
	{
		std::copy(indices_.begin(), indices_.end(), indexmap);
		indexBuff_->Unmap(0, nullptr);
	}

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeIB;
	ibView_.Format = DXGI_FORMAT_R16_UINT;
}

void ModelFBX::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView_);

	setMaterialLightTex();

	// �`��R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices_.size(), 1, 0, 0, 0);
}
