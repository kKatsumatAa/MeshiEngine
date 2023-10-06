#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Bullet :
	public Object
{
private:
	//��������
	float lifeTime_ = 300;

	//�����x�N�g��
	Vec3 directionVec_ = { 0,0,0 };

	//���L��
	IObject* owner_ = nullptr;

	Vec3 ownerPos_ = { 0,0,0 };

	//�O�̃t���[���̈ʒu
	Vec3 oldPos_ = { 0,0,0 };

	//�e��
	Object ballisticsObj_;
	float ballisticsLength = 0;
	const float BALLISTICS_LENGTH_MAX_ = 10;


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);

private:
	void Dead(const Vec3& interPos);

	//�e���p�̃I�u�W�F�N�g�̈ʒu�Ȃ�
	void BallisticsUpdate();

public:
	IObject* GetOwner() { return owner_; }

	const Vec3& GetOwnerPos() { return ownerPos_; }

public:

	bool Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);

	void Update() override;

	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
};

