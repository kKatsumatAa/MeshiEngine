#include "ConstBuffTransform.h"
#include "TextureManager.h"


void ConstBuffTransform::Initialize()
{
	HRESULT result;

	//�o�b�t�@�쐬
	{
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};

		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/;
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&cbHeapProp,//�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,//���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform_));
		assert(SUCCEEDED(result));

	}
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//�}�b�s���O
	assert(SUCCEEDED(result));
}

void ConstBuffTransform::DrawCommand(int32_t index)
{
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(index, constBuffTransform_->GetGPUVirtualAddress());
}