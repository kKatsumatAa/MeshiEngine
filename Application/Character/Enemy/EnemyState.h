#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"

class Enemy;


//素手か、銃を持ってるか等のステート
class EnemyState : public CharacterState
{
protected:
	Enemy* enemy_ = nullptr;

	const uint16_t BARE_HANDS_ATTR_TMP_ = COLLISION_ATTR_ITEMS | COLLISION_ATTR_LANDSHAPE;
	const uint16_t HAVE_WEAPON_ATTR_TMP_ = COLLISION_ATTR_ALLIES | COLLISION_ATTR_LANDSHAPE;

public:
	virtual ~EnemyState() { ; }

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	bool CheckEyeRayHit()override;

	//持ち主のいない銃が見えたらその座標、なければプレイヤーの座標
	Vec3 GetRayHitGunOrPlayerPos();

	//武器持ってるかどうかでステート変更
	void ChangeState();

public:
	virtual void Update();

};

//出現演出
class EnemyStateEmergeEffect : public EnemyState
{
private:
	//敵出現時演出用
	const float EMERGE_TIMER_MAX_ = 60;

public:
	void Initialize() override;
	void Update() override;
};

//素手状態
class EnemyStateBareHands :
	public CharacterStateBareHands,
	public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//銃持ってる
class EnemyStateHaveWeapon :
	public CharacterStateHaveWeapon,
	public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//被ダメージ始め
class EnemyStateDamagedBegin : public EnemyState
{
private:
	float timer_ = 0;
	const float TIMER_MAX_ = 20.0f;

public:
	void Initialize() override;
	void Update() override;
};

//被ダメージ終わり
class EnemyStateDamagedEnd : public EnemyState
{
private:
	float timer_ = 0;
	const float TIMER_MAX_ = 20.0f;

public:
	void Initialize() override;
	void Update() override;
};

//死亡時
class EnemyStateDead :
	public EnemyState,
	public CharacterStateDead
{
private:
	float timer_ = 0;
	const uint64_t PARTICLE_INTERVAL_ = 10;

public:
	void Initialize() override;
	void Update() override;
};