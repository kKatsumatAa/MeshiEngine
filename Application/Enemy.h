#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"


class Enemy :
	public Object
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.8f;
	bool isCantMove = false;

	//向きをプレイヤーに向けるためのクォータニオン
	Quaternion directionQua_;
	//初期向きのベクトル
	Vec3 directionVec = { 0,0,1.0f };
	//回転の時間
	float directionRotTime = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	//hp
	int8_t hp = 3;
	float damageCoolTime = 0;
	float velocityLength = 0;

	//銃
	Gun* gun_ = nullptr;

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, Gun* gun);

private:
	//移動
	void Move();

	//銃をノックバックして落とす
	void KnockBack(const CollisionInfo& info);

	//被弾時のパーティクル
	void DamageParticle(const CollisionInfo& info);

	//銃を解除
	void GunRelease(const Vec3& directionVec);

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, Gun* gun);

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

