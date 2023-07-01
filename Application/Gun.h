#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Gun :
	public Object
{
private:
	//攻撃のクールタイム
	const float SHOT_COOL_TIME_MAX_ = 35;
	float shotCoolTime_ = SHOT_COOL_TIME_MAX_;

	const float BULLET_VELOCITY_ = 1.8f;

	//弾の残量
	const int32_t BULLETS_TMP_ = 3;
	int32_t remainingBullets_ = BULLETS_TMP_;

	//球を打つか
	bool isAlreadyShot_ = false;

	//手を離れたときのベクトル
	Vec3 fallVec_ = { 0,0,0 };

	Vec3 localPos_ = { 0,0,0 };

	//発射位置
	Vec3 shotPos = { 0,0,0 };


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Gun> Create(std::unique_ptr<WorldMat> worldMat);

private:
	//所有者がいないときに飛んでいく
	void NoParentMove();

	void ParticleGenerate();

public:
	//射撃
	void Shot(const Vec3& directionVec, int32_t decreBullet = 1);

	//持ち主変更
	void ChangeOwner(WorldMat* parent);

public:
	//
	void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//ローカル座標セット
	void SetLocalPos(const Vec3& pos) { localPos_ = pos; }

	//投げる、被弾して落ちるときのスピード
	const Vec3& GetFallVelocity() { return fallVec_; }

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat) override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

