#pragma once
#include"TextureManager.h"

//�A�t�B���ϊ��s��p
class ConstBuffTransform
{
private:
	//�萔�o�b�t�@�p�f�[�^�\��
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //�r���[�v���W�F�N�V�����s��
		XMMATRIX world;    //���[���h�s��
		XMFLOAT3 cameraPos;//�J�������W�i���[���h���W�j
	};

public:
	ComPtr < ID3D12Resource> constBuffTransform_ = nullptr;//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ConstBufferDataTransform* constMapTransform_ = nullptr;//�萔�o�b�t�@�̃}�b�s���O�p�|�C���^

	ConstBuffTransform();
	void Initialize();
};