#pragma once
#include"CollisionManager.h"


class Player;


//�f�肩�A�e�������Ă邩���̃X�e�[�g
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

//�f����
class PlayerStateBareHands :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};

//�e�����Ă�
class PlayerStateHaveGun :public PlayerState
{

public:
	void Initialize() override;
	void Update() override;
};
