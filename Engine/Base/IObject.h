#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"
#include "LightManager.h"


//�R���C�_�[�̐e�N���X�O���錾
class BaseCollider;

//�I�u�W�F�N�g�e�N���X
class IObject
{
protected://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�v�C��
	enum ROOTPARAM_NUM
	{
		COLOR,
		TEX,
		MATRIX,
		MESH_MAT,
		MATERIAL,
		LIGHT,
		EFFECT,
		SKIN,
		DISSOLVE,
		SPECULAR_MAP,
		NORM_MAP
	};

protected://�p����܂Ō��J
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;
	//�萔�o�b�t�@�̐���
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//���C�g
	static LightManager* sLightManager_;

protected:
	//���[���h�s��p�N���X
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//�e�C���X�^���X
	IObject* parentObj_ = nullptr;

protected:
	//�N���X��(�f�o�b�O�p)
	std::string objName_;
	//�����Ă�t���O
	bool isAlive_ = true;
	//�`��A�X�V���邩�ǂ���
	bool isValid_ = true;
	//���x
	Vec3 velocity_ = { 0,0,0 };
	//�R���C�_�[
	std::unique_ptr<BaseCollider> collider_ = nullptr;


//�֐�------------------------------------------------------------------------------
protected:
	//���ʂ̏���������
	void InitializeCommon(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	virtual void Update();

	virtual void Draw();

public:
	//�s����X�V�A����ɔ����R���C�_�[��
	void WorldMatColliderUpdate();

	//�f�o�b�O�p�̕\��
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

	//�p�C�v���C���̐ݒ�
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe);

public:
	//�Փˎ��R�[���o�b�N�֐�
	virtual void OnCollision(const CollisionInfo& info) {}

public:
	//�����Ă邩
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//�`��A�X�V���邩�ǂ���
	virtual void SetIsValid(bool isValid);
	bool GetIsValid() { return isValid_; }

	//�X�s�[�h�i�����蔻��Ɏg���j
	inline void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	inline const Vec3& GetVelocity() { return velocity_; }


public:
	//�ʒu
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z = pos; }
	Vec3 GetTrans() { return worldMat_->trans_; }
	//�X�P�[��
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z = scale; }
	const Vec3& GetScale() { return worldMat_->scale_; }
	//��]
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z = rot; }
	const Vec3& GetRot() { return worldMat_->rot_; }
	//�s����X�V
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//���[���h�s��̒��g�R�s�[
	void SetWorldMat(std::unique_ptr<WorldMat> worldMat) { worldMat_ = std::move(worldMat); }
	//�e
	void SetParent(Object* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//���[���h�s��p�N���X�̃C���X�^���X���擾
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//�e�q�֌W���l�������ʒu���Q�b�g
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }
	//���[���h�s��̎擾
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//�N�H�[�^�j�I���n
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

public:
	//�I�u�W�F�N�g���O
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }

	//�R���C�_�[
	BaseCollider* GetCollider() { return collider_.get(); }
	//�R���C�_�[�̃Z�b�g
	void SetCollider(std::unique_ptr<BaseCollider> collider);
	//�����L���ɂ��邩
	void SetColliderIsValid(bool isValid);

public:
	//�F��Ԃ�
	const Vec4& GetColor() { return constMapMaterial_->color; }

};

void SetNormDigitalMat(DirectX::XMMATRIX& mat);

void Error(bool filed, ID3DBlob* errorBlob);