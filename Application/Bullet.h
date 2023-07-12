#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Bullet :
	public Object
{
private:
	//生存時間
	float lifeTime_ = 300;

	//方向ベクトル
	Vec3 directionVec_ = { 0,0,0 };

	//所有者
	Object* owner_ = nullptr;

	Vec3 ownerPos_ = { 0,0,0 };

	//前のフレームの位置
	Vec3 oldPos_ = { 0,0,0 };


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner);

private:
	void Dead(const Vec3& interPos);

public:
	Object* GetOwner() { return owner_; }

	const Vec3& GetOwnerPos() { return ownerPos_; }

public:

	bool Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner);

	void Update() override;

	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
};

