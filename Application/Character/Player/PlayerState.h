#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"


class Player;


//‘fè‚©Ae‚ğ‚Á‚Ä‚é‚©“™‚ÌƒXƒe[ƒg
class PlayerState : public CharacterState
{
protected:
	Player* player_ = nullptr;

public:
	virtual ~PlayerState() { ; }

	void SetPlayer(Player* player) { player_ = player; }

	bool CheckEyeRayHit()override;

	virtual void Initialize()override = 0;
	virtual void Update()override = 0;
};

//‘fèó‘Ô
class PlayerStateBareHands :
	public PlayerState,
	public CharacterStateBareHands

{

public:
	void Initialize() override;
	void Update() override;
};

//e‚Á‚Ä‚é
class PlayerStateHaveWeapon :
	public PlayerState,
	public CharacterStateHaveWeapon
{

public:
	void Initialize() override;
	void Update() override;
};
