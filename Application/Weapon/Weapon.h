#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Weapon :
	public Object
{
protected:
	//�t���[�����Ƃ̔{��
	const float FRAME_VEL_EXTEND_ = 0.9f;

protected:
	//��𗣂ꂽ�Ƃ��̃x�N�g��
	Vec3 fallVec_ = { 0,0,0 };
	const float FALL_VEC_Y_MIN_ = -4.0f;
	//���[�J�����W
	Vec3 localPos_ = { 0,0,0 };
	//������
	Object* owner_ = nullptr;

	//�O��̈ʒu
	Vec3 oldPos_ = { 0,0,0 };

	//�������Ă��邩
	bool isThrowing_ = false;

	//�N�[���^�C��
	float attackCoolTime_ = 0;

	//�d��
	const float GRAVITY_TMP_ = 0.15f;
	const float GRAVITY_MAX_ = 3.0f;

public:
	Weapon() { ; }
	virtual ~Weapon() { ; }

protected:
	//�n�`�Ɠ����������ɌĂяo��
	virtual void OnLandShape(const Vec3& interPos) = 0;

	//���L�҂����Ȃ��Ƃ��ɔ��ł���
	virtual void NoParentMove();

	virtual void ParticleGenerate() = 0;

public:
	//�U��
	virtual void Attack(const Vec3& directionVec, int32_t decreBullet = 1, Object* owner = nullptr) = 0;

	//������ύX
	virtual void ChangeOwner(Object* parent);

	//���葮��
	void SetAttribute(uint16_t attribute) { GetCollider()->SetAttribute(attribute); }

public:
	//
	virtual void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//���[�J�����W�Z�b�g
	virtual  void SetLocalPos(const Vec3& pos) {
		localPos_ = pos;
	}

	Vec3 GetLocalPos() { return localPos_; }

	//������A��e���ė�����Ƃ��̃X�s�[�h
	virtual const Vec3& GetFallVelocity() { return fallVec_; }

	bool GetIsThrowing() { return isThrowing_; }
	void SetIsThrowing(bool isThrowing) { isThrowing_ = isThrowing; }

	//�N�[���^�C���̏I���܂ł̊���(0~1.0f)
	virtual float GetAttackCoolTimeRatio() = 0;

public:

	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model) override = 0;

	virtual void Update();

	virtual void OnCollision(const CollisionInfo& info) override = 0;

public:
	//�Â����W�̍X�V
	void OldPosUpdate() { oldPos_ = GetWorldTrans(); }
};

