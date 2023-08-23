#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"
#include "Character.h"


class EnemyState;

class Enemy :
	public Character
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.8f;
	bool isCantMove = false;

	//ウェーブ番号
	int32_t waveNum_ = -1;

	//向きをプレイヤーに向けるためのクォータニオン
	Quaternion directionQua_;
	//回転の時間
	float directionRotTime_ = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	Vec3 directionVec_;

	float damageCoolTime_ = 0;
	float velocityLength_ = 0;

	const int8_t HP_TMP_ = 3;

	const float KNOCK_BACK_POW_ = 0.165f;

	const float DISSOLVE_POW_ = 0.6f;

	const float WEAPON_FALL_VEL_EXTEND_ = 1.6f;

	//ステート
	std::unique_ptr<EnemyState> state_ = nullptr;

public:
	static const float S_LENGTH_MAX_;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon);

private:
	//銃をノックバックして落とす
	void KnockBack(const CollisionInfo& info);

	//被弾時のパーティクル
	void DamageParticle(const CollisionInfo& info);

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon);

	void Update() override;

	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;

	void ChangeEnemyState(std::unique_ptr<EnemyState> state);

public:
	//移動
	void Move(const Vec3& targetPos);
	//攻撃
	void Attack(const Vec3& targetPos);
	//向きを変更
	void DirectionUpdate(const Vec3& targetPos);

public:
	int32_t GetWaveNum() { return waveNum_; }
};

