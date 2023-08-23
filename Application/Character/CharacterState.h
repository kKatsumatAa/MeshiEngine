#pragma once
#include"CollisionManager.h"


class Character;


//�f�肩�A�e�������Ă邩���̃X�e�[�g
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

//�f����
class CharacterStateBareHands
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//�e�����Ă�
class CharacterStateHaveWeapon
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};
