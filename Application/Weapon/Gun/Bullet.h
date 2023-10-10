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
	IObject* owner_ = nullptr;

	Vec3 ownerPos_ = { 0,0,0 };

	//前のフレームの位置
	Vec3 oldPos_ = { 0,0,0 };

	//弾道
	Object ballisticsObj_;
	float ballisticsLength = 0;
	const float BALLISTICS_LENGTH_MAX_ = 10;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);

private:
	void Dead(const Vec3& interPos);

	//弾道用のオブジェクトの位置など
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

