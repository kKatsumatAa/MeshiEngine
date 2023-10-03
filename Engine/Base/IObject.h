#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"


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

private:
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

private:
	//�e�C���X�^���X
	IObject* parentObj_ = nullptr;

protected://�p����܂Ō��J
	//�N���X��(�f�o�b�O�p)
	std::string objName_;
	//�R���C�_�[
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//�p�C�v���C���̐ݒ�
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe);

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
	//
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//�e�q�֌W���l�������ʒu���Q�b�g
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }

	//�I�u�W�F�N�g���O
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }
	//�R���C�_�[
	BaseCollider* GetCollider() { return collider_.get(); }

public:

public:

};