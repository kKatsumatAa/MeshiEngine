#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Weapon.h"


class Gun :
	public Weapon
{
private:
	//攻撃のクールタイム
	const float SHOT_COOL_TIME_MAX_ = 35;
	float shotCoolTime_ = SHOT_COOL_TIME_MAX_;

	const float BULLET_VELOCITY_ = 2.3f;

	//弾の残量
	const int32_t BULLETS_TMP_ = 3;
	int32_t remainingBullets_ = BULLETS_TMP_;

	//発射位置
	Vec3 shotPos_ = { 0,0,0 };


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Gun> Create(std::unique_ptr<WorldMat> worldMat);

private:
	////所有者がいないときに飛んでいく
	//void NoParentMove() override;

	void ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor);
	void ParticleGenerate()override { ; }

public:
	//射撃
	void Attack(const Vec3& directionVec, int32_t decreBullet = 1, Object* owner = nullptr) override;

	//持ち主変更
	void ChangeOwner(WorldMat* parent) override;

	//
	int32_t GetBulletNum() { return remainingBullets_; }

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat) override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

