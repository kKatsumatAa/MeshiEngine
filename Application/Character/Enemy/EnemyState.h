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
