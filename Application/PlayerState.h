#pragma once
#include"CollisionManager.h"


class Player;


//‘fè‚©Ae‚ğ‚Á‚Ä‚é‚©“™‚ÌƒXƒe[ƒg
class PlayerState
{
protected:
	Player* player_ = nullptr;
	int32_t timer_ = 0;
	RaycastHit info_;

public:
	void SetPlayer(Player* player) { player_ = player; }

	bool CheckEyeRayHit();

	void ThrowGun();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
};

//‘fèó‘Ô
class PlayerStateBareHands :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};

//e‚Á‚Ä‚é
class PlayerStateHaveGun :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};
