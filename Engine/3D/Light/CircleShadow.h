#pragma once
#include<DirectXMath.h>


/// <summary>
/// �ۉe
/// </summary>
class CircleShadow
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
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad1;
		XMFLOAT2 factorAngleCos;
		uint32_t active;
		float pad2;
	};


public://�����o�֐�
	inline void SetDir(const XMVECTOR& dir) { dir_ = DirectX::XMVector3Normalize(dir); }
	inline const XMVECTOR& GetDir() { return dir_; }

	inline void SetCasterPos(const XMFLOAT3& casterPos) { casterPos_ = casterPos; }
	inline const XMFLOAT3& GetCasterPos() { return casterPos_; }

	inline void SetDistanceCasterLight(float distanceCasterLight) { distanceCasterLight_ = distanceCasterLight; }
	inline float GetDistanceCasterLight() { return distanceCasterLight_; }

	inline void SetAtten(const XMFLOAT3& atten) { atten_ = atten; }
	inline const XMFLOAT3& GetAtten() { return atten_; }

	inline void SetFactorAngleCos(const XMFLOAT2& FactorAngleCos) {
		factorAngleCos_.x = cosf(DirectX::XMConvertToRadians(FactorAngleCos.x));
		factorAngleCos_.y = cosf(DirectX::XMConvertToRadians(FactorAngleCos.y));
	};
	inline const XMFLOAT2& GetFactorAngleCos() {
		return factorAngleCos_;
	};

	inline void SetLightActive(bool active) {
		active_ = active;
	};
	inline bool GetLightActive() {
		return active_;
	};

private://�����o�ϐ�
	//�����i�P�ʃx�N�g���j
	XMVECTOR dir_ = { 1.0f,0,0 };
	//�L���X�^�[�ƃ��C�g�̋���
	float distanceCasterLight_ = 100.0f;
	//�L���X�^�[���W�i���[���h���W�n�j
	XMFLOAT3 casterPos_ = { 0,0,0 };
	//���������W��
	XMFLOAT3 atten_ = { 0.5f,0.6f,0.0f };
	//�����p�x
	XMFLOAT2 factorAngleCos_ = { 0.2f,0.5f };
	//�L���t���O
	bool active_ = false;
};

