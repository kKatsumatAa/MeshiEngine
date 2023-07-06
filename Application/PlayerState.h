#pragma once
#include"CollisionManager.h"


class Player;


//素手か、銃を持ってるか等のステート
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

//素手状態
class PlayerStateBareHands :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};

//銃持ってる
class PlayerStateHaveGun :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};
