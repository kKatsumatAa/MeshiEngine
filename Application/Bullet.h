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

	//生きてるか
	bool isAlive_ = true;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime);

private:

public:
	bool GetIsAlive() { return isAlive_; }

public:

	bool Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime);

	void Update() override;

	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
};

