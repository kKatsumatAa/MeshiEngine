#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Weapon :
	public Object
{
protected:
	//��𗣂ꂽ�Ƃ��̃x�N�g��
	Vec3 fallVec_ = { 0,0,0 };
	//���[�J�����W
	Vec3 localPos_ = { 0,0,0 };


public:
	/*/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	virtual std::unique_ptr<Weapon> Create(std::unique_ptr<WorldMat> worldMat) = 0;*/

protected:
	//���L�҂����Ȃ��Ƃ��ɔ��ł���
	virtual void NoParentMove();

	virtual void ParticleGenerate() = 0;

public:
	//�ˌ�
	virtual void Attack(const Vec3& directionVec, int32_t decreBullet = 1) = 0;

	//������ύX
	virtual void ChangeOwner(WorldMat* parent) = 0;

public:
	//
	virtual void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//���[�J�����W�Z�b�g
	virtual  void SetLocalPos(const Vec3& pos) {
		localPos_ = pos;
	}

	//������A��e���ė�����Ƃ��̃X�s�[�h
	virtual const Vec3& GetFallVelocity() { return fallVec_; }

public:

	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat) override = 0;

	virtual void Update() override = 0;

	virtual void OnCollision(const CollisionInfo& info) override = 0;
};

