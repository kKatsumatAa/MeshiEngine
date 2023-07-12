#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"
#include "Character.h"


class Enemy :
	public Character
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

	float damageCoolTime = 0;
	float velocityLength = 0;

	const int8_t HP_TMP_ = 3;

	const float KNOCK_BACK_POW_ = 0.165f;

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

private:
	//移動
	void Move();

	//銃をノックバックして落とす
	void KnockBack(const CollisionInfo& info);

	//被弾時のパーティクル
	void DamageParticle(const CollisionInfo& info);

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

