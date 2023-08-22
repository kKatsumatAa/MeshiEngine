#pragma once
#include"CollisionManager.h"


class Enemy;


//�f�肩�A�e�������Ă邩���̃X�e�[�g
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

//�f����
class EnemyStateBareHands :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//�e�����Ă�
class EnemyStateHaveWeapon :public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};
