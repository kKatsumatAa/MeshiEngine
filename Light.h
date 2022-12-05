#pragma once
#include "DirectX.h"

/// <summary>
/// �Ɩ�
/// </summary>
class Light
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private://�ÓI�����o�ϐ�
	static ID3D12Device* device_;

private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//���C�g��������
	XMVECTOR lightdir = { 1,0,0,0 };
	//���C�g�F
	XMFLOAT3 lightColor = { 1,1,1 };
	//�_�[�e�B�t���O
	bool dirty = false;

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMVECTOR lightv;     //���C�g�̕�����\���x�N�g��
		XMFLOAT3 lightColor; //���C�g�̐F
	};


public://�ÓI�����o�֐�
	static void StaticInitialize();

public:
	void Initialize();
};

