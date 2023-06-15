#pragma once
#include<DirectXMath.h>
#include <wrl.h>
using namespace Microsoft::WRL;

/// <summary>
/// �_����
/// </summary>
class PointLight
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

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		uint32_t active;
	};

private://�����o�ϐ�
	//���C�g���W�i���[���h���W�n�j
	XMFLOAT3 lightpos_ = { 0,0,0 };
	//���C�g�F
	XMFLOAT3 lightcolor_ = { 1.0f,1.0f,1.0f };
	//���C�g���������W��
	XMFLOAT3 lightatten_ = { 1.0f,1.0f,1.0f };
	//�L���t���O
	bool active_ = false;



public://�����o�֐�
	inline void SetLightPos(const XMFLOAT3& lightpos) { lightpos_ = lightpos; }
	inline const XMFLOAT3& GetLightPos() { return lightpos_; }
	inline void SetLightColor(const XMFLOAT3& lightcolor) { lightcolor_ = lightcolor; }
	inline const XMFLOAT3& GetLightColor() { return lightcolor_; }
	inline void SetLightAtten(const XMFLOAT3& lightatten) { lightatten_ = lightatten; }
	inline const XMFLOAT3& GetLightAtten() { return lightatten_; }
	inline void SetActive(bool active) { active_ = active; }
	inline bool GetActive() { return active_; }
};

