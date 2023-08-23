#pragma once
#include"CollisionManager.h"


class Character;


//‘fè‚©Ae‚ğ‚Á‚Ä‚é‚©“™‚ÌƒXƒe[ƒg
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

//‘fèó‘Ô
class CharacterStateBareHands
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//e‚Á‚Ä‚é
class CharacterStateHaveWeapon
{

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
};
