#pragma once
#include"CollisionManager.h"


class Character;


//素手か、銃を持ってるか等のステート
class CharacterState
{
protected:
	float timer_ = 0;
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

//------------------------------
//構えの親ステート
class CharacterStanceState
{
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//攻撃構え始め
class CharacterStateAttackStanceBegin : public CharacterStanceState
{
public:
	virtual void Initialize()override = 0;
	virtual void Update()override = 0;
};

//攻撃構え終わり
class CharacterStateAttackStanceEnd : public CharacterStanceState
{
public:
	virtual void Initialize()override = 0;
	virtual void Update()override = 0;
};

//---------------------------
//死亡時
class CharacterStateDead
{
protected:
	float t_ = 0;

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};