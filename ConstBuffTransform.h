#pragma once
#include"DirectX.h"

class ConstBuffTransform
{
private:
	//�萔�o�b�t�@�p�f�[�^�\��
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D�ϊ��s��
	};
	//05_02
	
	/*HRESULT result;
	ID3D12Device* device = nullptr;*/
	//D3D12_RESOURCE_DESC resDesc{};

public:
	ComPtr < ID3D12Resource> constBuffTransform = nullptr;//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ConstBufferDataTransform* constMapTransform = nullptr;//�萔�o�b�t�@�̃}�b�s���O�p�|�C���^

	ConstBuffTransform(/*D3D12_RESOURCE_DESC& resDesc,*/ /*Directx& directx*/);
	void Initialize(Directx& directx);
};