#pragma once
#include"CollisionManager.h"


class Character;


//素手か、銃を持ってるか等のステート
class CharacterState
{
protected:
	int32_t timer_ = 0;
	RaycastHit info_;

public:
	virtual ~CharacterState() { ; }

	virtual bool CheckEyeRayHit() = 0;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//素手状態
class CharacterStateBareHands
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//銃持ってる
class CharacterStateHaveWeapon
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};
