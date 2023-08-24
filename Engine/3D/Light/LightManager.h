#pragma once
#include "DirectXWrapper.h"
#include "PointLight.h"
#include "DirLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"


class LightManager
{
public:
	//�_�����̐�
	static const int32_t S_POINT_LIGHT_NUM_ = 12;

private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private://�萔

	//���s����
	static const int32_t S_DIR_LIGHT_NUM_ = 3;

	//�X�|�b�g���C�g�̐�
	static const int32_t S_SPOT_LIGHT_NUM_ = 3;

	//�ۉe�̐�
	static const int32_t S_CIRCLE_SHADOW_NUM_ = 1;

private://�ÓI�����o�ϐ�
	static ID3D12Device* sDevice_;

private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;
	//�_�[�e�B�t���O
	bool dirty_ = false;
	// �����̐F
	XMFLOAT3 ambientColor_ = { 1,1,1 };
	XMFLOAT3 diffuseColor_ = { 1,1,1 };
	XMFLOAT3 specularColor_ = { 1,1,1 };
	//�_�����̔z��
	PointLight pointLights_[S_POINT_LIGHT_NUM_];
	// ���s�����̔z��
	DirLight dirLights_[S_DIR_LIGHT_NUM_];
	//�X�|�b�g���C�g�̔z��
	SpotLight spotLights_[S_SPOT_LIGHT_NUM_];
	//�ۉe�̔z��
	CircleShadow circleShadows_[S_CIRCLE_SHADOW_NUM_];

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����(���s�����A�_�����̔z����܂Ƃ߂đ���)
	struct ConstBufferData
	{
		// �����̐F
		XMFLOAT3 ambientColor;
		float pad1;
		// diffuse�̐F
		XMFLOAT3 diffuseColor;
		float pad2;
		// specular�̐F
		XMFLOAT3 specularColor;
		float pad3;

		// ���s�����̔z��
		DirLight::ConstBufferData dirLights[S_DIR_LIGHT_NUM_];
		//�_�����p
		PointLight::ConstBufferData pointLights[S_POINT_LIGHT_NUM_];
		//�X�|�b�g���C�g�p
		SpotLight::ConstBufferData spotLights[S_SPOT_LIGHT_NUM_];
		//�ۉe�p
		CircleShadow::ConstBufferData circleShadows[S_CIRCLE_SHADOW_NUM_];
	};


private:
	/// <summary>
/// �萔�o�b�t�@�]��
/// </summary>
	void TransferConstBuffer();

public://�ÓI�����o�֐�
	static void StaticInitialize();

public:
	void Initialize();

	void InitializeActive();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��i�`��͂����A�萔�o�b�t�@�r���[�̃Z�b�g�̂ݍs���j
	/// </summary>
	/// <param name="rootParamaterIndex"></param>
	void Draw(uint32_t rootParamaterIndex);

	//
	void DrawImGui();

	/// <summary>
	/// �W���̃��C�g�ݒ�
	/// </summary>
	void DefaultLightSetting();

	/// <summary>
	/// �����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="color">���C�g�F</param>
	void SetAmbientColor(const XMFLOAT3& color);

	void SetDiffuseColor(const XMFLOAT3& color);

	void SetSpecularColor(const XMFLOAT3& color);

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	static std::unique_ptr<LightManager> Create();

	/// <summary>
		/// ���s�����̗L���t���O���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="active">�L���t���O</param>
	void SetDirLightActive(int32_t index, bool active);

	/// <summary>
	/// ���s�����̃��C�g�������Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightdir">���C�g����</param>
	void SetDirLightDir(int32_t index, const XMVECTOR& lightdir);

	/// <summary>
	/// ���s�����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightcolor">���C�g�F</param>
	void SetDirLightColor(int32_t index, const XMFLOAT3& lightcolor);

	///�|�C���g���C�g�p
	void SetPointLightActive(int32_t index, bool active);
	void SetPointLightPos(int32_t index, const XMFLOAT3& pos);
	void SetPointLightColor(int32_t index, const XMFLOAT3& color);
	void SetPointLightAtten(int32_t index, const XMFLOAT3& atten);

	///�X�|�b�g���C�g�p
	void SetSpotLightActive(int32_t index, bool active);
	//���C�g�̕������Z�b�g
	void SetSpotLightDir(int32_t index, const XMVECTOR& lightdir);
	void SetSpotLightPos(int32_t index, const XMFLOAT3& lightpos);
	void SetSpotLightColor(int32_t index, const XMFLOAT3& lightcolor);
	//�����W�����Z�b�g
	void SetSpotLightAtten(int32_t index, const XMFLOAT3& lightAtten);
	//�����J�n�A�I���p�x
	void SetSpotLightFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

	///�ۉe�p
	void SetCircleShadowActive(int32_t index, bool active);
	//�e�̔������̃I�u�W�F�N�g�̍��W
	void SetCircleShadowCasterPos(int32_t index, const XMFLOAT3& casterPos);
	//���C�g�̕������Z�b�g
	void SetCircleShadowDir(int32_t index, const XMVECTOR& lightdir);
	//�e���烉�C�g�܂ł̋���
	void SetCircleShadowDistanceCasterLight(int32_t index, float distanceCasterLight);
	//�����W�����Z�b�g
	void SetCircleShadowAtten(int32_t index, const XMFLOAT3& lightAtten);
	//�����J�n�A�I���p�x
	void SetCircleShadowFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

public:
	//�g�p����ĂȂ��|�C���g���C�g�̃C���f�b�N�X
	bool GetDoNotUseDirLightIndex(int32_t& index);
	bool GetDoNotUsePointLightIndex(int32_t& index);
	bool GetDoNotUseSpotLightIndex(int32_t& index);
};

