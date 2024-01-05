/*
* @file EnemyState.h
* @brief 敵のステート（殴りか、銃持ってるか、死んでる演出中かなど）
*/

#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"

class Enemy;


//素手か、銃を持ってるか等のステート
class EnemyState : public CharacterState
{
protected:
	const float DAMAGED_ANIME_SPEED_RATE_ = 0.3f;
	const uint16_t BARE_HANDS_ATTR_TMP_ = COLLISION_ATTR_ITEMS | COLLISION_ATTR_LANDSHAPE;
	const uint16_t HAVE_WEAPON_ATTR_TMP_ = COLLISION_ATTR_ALLIES | COLLISION_ATTR_LANDSHAPE;
protected:
	Enemy* enemy_ = nullptr;


public:
	//デストラクタ
	virtual ~EnemyState() { ; }

	//敵のポインタセット
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	//正面ベクトル方向に何かあるか
	bool CheckEyeRayHit()override;

	//持ち主のいない銃が見えたらその座標、なければプレイヤーの座標
	Vec3 GetRayHitGunOrPlayerPos();
	//プレイヤーが視界に入っているか
	bool GetPlayerVisually();
	//武器持ってるかどうかでステート変更
	void ChangeState();

public:
	//更新
	virtual void Update();
};

//出現演出
class EnemyStateEmergeEffect : public EnemyState
{
private:
	//敵出現時演出用
	const float EMERGE_TIMER_MAX_ = 60;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//素手状態-----------------------------------------------
class EnemyStateBareHands :
	public CharacterStateBareHands,
	public EnemyState
{

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//銃持ってる-------------------------------------------
class EnemyStateHaveWeapon :
	public CharacterStateHaveWeapon,
	public EnemyState
{
private:
	static bool isAttacking_;

public:
	//初期化
	virtual void Initialize() override;
	//更新
	virtual void Update() override;
};

//銃持って撃つ
class EnemyStateHaveWeaponAndAttack : public EnemyStateHaveWeapon
{
public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//銃持っててプレイヤーが視界にない
class EnemyStateHaveWeaponAndMove : public EnemyStateHaveWeapon
{
public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//-----------------------------------------------
//被ダメージ始め
class EnemyStateDamagedBegin : public EnemyState
{
public:
	const float TIME_RATIO_MAX_ = 1.0f;
	const float TIMER_MAX_ = 20.0f;
private:
	float timer_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//被ダメージ終わり
class EnemyStateDamagedEnd : public EnemyState
{
public:
	const float TIME_RATIO_MAX_ = 1.0f;
	const float TIMER_MAX_ = 20.0f;
private:
	float timer_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//死亡時
class EnemyStateDead :
	public EnemyState,
	public CharacterStateDead
{
public:
	const float TIME_RATIO_MAX_ = 1.0f;
	const uint64_t PARTICLE_INTERVAL_ = 10;
	const float DEAD_PARTICLE_NUM_ = 10.0f;
private:
	float timer_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};