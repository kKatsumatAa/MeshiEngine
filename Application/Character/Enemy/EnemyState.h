#pragma once
#include"CollisionManager.h"


class Enemy;


//‘fè‚©Ae‚ğ‚Á‚Ä‚é‚©“™‚ÌƒXƒe[ƒg
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

//‘fèó‘Ô
class EnemyStateBareHands :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//e‚Á‚Ä‚é
class EnemyStateHaveWeapon :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};
