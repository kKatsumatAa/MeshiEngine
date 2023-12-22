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
	Enemy* enemy_ = nullptr;

	const uint16_t BARE_HANDS_ATTR_TMP_ = COLLISION_ATTR_ITEMS | COLLISION_ATTR_LANDSHAPE;
	const uint16_t HAVE_WEAPON_ATTR_TMP_ = COLLISION_ATTR_ALLIES | COLLISION_ATTR_LANDSHAPE;

public:
	virtual ~EnemyState() { ; }

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	bool CheckEyeRayHit()override;

	//持ち主のいない銃が見えたらその座標、なければプレイヤーの座標
	Vec3 GetRayHitGunOrPlayerPos();
	//プレイヤーが視界に入っているか
	bool GetPlayerVisually();
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

//素手状態-----------------------------------------------
class EnemyStateBareHands :
	public CharacterStateBareHands,
	public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//素手攻撃状態始め
class EnemyStateBareHandsAttackBegin :
	public CharacterStateBareHands,
	public EnemyState
{
private:
	const float TIME_ = 20.0f;
	float t_ = 0;

public:
	void Initialize() override;
	void Update() override;
};


//素手攻撃状態
class EnemyStateBareHandsAttack :
	public CharacterStateBareHands,
	public EnemyState
{
private:
	const float ATTACK_TIME_ = 45.0f;
	float t_ = 0;

public:
	void Initialize() override;
	void Update() override;
};

//素手攻撃おわり
class EnemyStateBareHandsAttackEnd :
	public CharacterStateBareHands,
	public EnemyState
{
private:
	const float TIME_ = 20.0f;
	float t_ = 0;

public:
	void Initialize() override;
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
	virtual void Initialize() override;
	virtual void Update() override;
};

//銃持って撃つ
class EnemyStateHaveWeaponAndAttack : public EnemyStateHaveWeapon
{
public:
	void Initialize() override;
	void Update() override;
};

//銃持っててプレイヤーが視界にない
class EnemyStateHaveWeaponAndMove : public EnemyStateHaveWeapon
{
public:
	void Initialize() override;
	void Update() override;
};

//-------------------------
//攻撃構えの親ステート
class EnemyStateAttackStance :
	public EnemyState
{
protected:
	//最大時間
	const float TIMER_MAX_ = 40.0f;
	float t_ = 0;
	//足す角度
	static Vec3 ANGLE_MAX_;
	const std::string MOVE_NODE_NAME_ = "RightArm";
	//線形補間用の角度
	Vec3 stanceEndRot_ = { 0,0,0 };
	Vec3 stanceBeginRot_ = { 0,0,0 };

public:
	virtual void Initialize() override = 0;
	virtual void Update() override;

	static void DrawImgui();
};

//攻撃構え始め
class EnemyStateAttackStanceBegin :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceBegin
{
public:
	void Initialize() override;
	void Update() override;
};

//攻撃構え終わり
class EnemyStateAttackStanceEnd :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
public:
	void Initialize() override;
	void Update() override;
};

//----------------
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