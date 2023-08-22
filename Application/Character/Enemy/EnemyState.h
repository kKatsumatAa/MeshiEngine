#pragma once
#include"CollisionManager.h"


class Enemy;


//素手か、銃を持ってるか等のステート
class EnemyState
{
protected:
	Enemy* enemy_ = nullptr;
	int32_t timer_ = 0;
	RaycastHit info_;

public:
	virtual ~EnemyState() { ; }

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	bool CheckEyeRayHit();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//素手状態
class EnemyStateBareHands :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//銃持ってる
class EnemyStateHaveWeapon :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};
