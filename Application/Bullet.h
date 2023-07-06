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

	//�����Ă邩
	bool isAlive_ = true;

	//���L��
	Object* owner_ = nullptr;

	Vec3 ownerPos_ = { 0,0,0 };


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner);

private:

public:
	bool GetIsAlive() { return isAlive_; }

	Object* GetOwner() { return owner_; }

	const Vec3& GetOwnerPos() { return ownerPos_; }

public:

	bool Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner);

	void Update() override;

	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
};

