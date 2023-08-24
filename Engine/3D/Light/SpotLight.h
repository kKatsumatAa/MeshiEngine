#pragma once

#include<DirectXMath.h>

/// <summary>
/// �X�|�b�g���C�g
/// </summary>
class SpotLight
{
private://�G�C���A�X
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
		XMVECTOR lightv;
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		float pad3;
		XMFLOAT2 lightfactoranglecos;
		uint32_t active;
		float pad4;
	};

public://�����o�֐�

	inline void SetLightDir(const XMVECTOR& lightdir) {
		lightdir_ = DirectX::XMVector3Normalize(lightdir);
	};
	inline const XMVECTOR& GetLightDir() {
		return lightdir_;
	};
	inline void SetLightPos(const XMFLOAT3& lightpos) {
		lightpos_ = lightpos;
	};
	inline const XMFLOAT3& GetLightPos() {
		return lightpos_;
	};
	inline void SetLightColor(const XMFLOAT3& lightcolor) {
		lightcolor_ = lightcolor;
	};
	inline const XMFLOAT3& GetLightColor() {
		return lightcolor_;
	};
	inline void SetLightAtten(const XMFLOAT3& lightAtten) {
		lightAtten_ = lightAtten;
	};
	inline const XMFLOAT3& GetLightAtten() {
		return lightAtten_;
	};
	inline void SetLightFactorAngleCos(const XMFLOAT2& lightFactoeAngleCos) {
		lightFactorAngleCos_.x = cosf(DirectX::XMConvertToRadians(lightFactoeAngleCos.x));
		lightFactorAngleCos_.y = cosf(DirectX::XMConvertToRadians(lightFactoeAngleCos.y));
	};
	inline const XMFLOAT2& GetLightFactorAngleCos() {
		return lightFactorAngleCos_;
	};
	inline void SetActive(bool active) {
		active_ = active;
	};
	inline bool GetActive() {
		return active_;
	};

	void DrawImGui(int32_t index);


private://�����o�ϐ�
	//���C�g����
	XMVECTOR lightdir_ = { 1.0f,0,0 };
	//���C�g���W�i���[���h���W�n�j
	XMFLOAT3 lightpos_ = { 0,0,0 };
	//���C�g�F
	XMFLOAT3 lightcolor_ = { 1,1,1 };
	//���C�g���������W��
	XMFLOAT3 lightAtten_ = { 1.0f,1.0f,1.0f };
	//���C�g�����p�x�i�J�n�p�x�A�I���p�x�j
	XMFLOAT2 lightFactorAngleCos_ = { 0.5f,0.2f };
	//�L���t���O
	bool active_ = false;

};

