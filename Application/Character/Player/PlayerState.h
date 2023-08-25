#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"


class Player;


//�f�肩�A�e�������Ă邩���̃X�e�[�g
class PlayerState : public CharacterState
{
protected:
	Player* player_ = nullptr;

public:
	virtual ~PlayerState() { ; }

	void SetPlayer(Player* player) { player_ = player; }

	bool CheckEyeRayHit()override;

	virtual void Initialize()override = 0;
	virtual void Update()override;
};

//�f����
class PlayerStateBareHands :
	public PlayerState,
	public CharacterStateBareHands

{

public:
	void Initialize() override;
	void Update() override;
};

//�e�����Ă�
class PlayerStateHaveWeapon :
	public PlayerState,
	public CharacterStateHaveWeapon
{

public:
	void Initialize() override;
	void Update() override;
};

//���S���o
class PlayerStateDeadEffect : public PlayerState
{
private:
	const int32_t TIMER_MAX_ = 70;
	Vec3 targetPos_;
	Vec3 dir_;
	const float LENGTH_MIN_ = 10.0f;

public:
	void Initialize() override;
	void Update() override;
};
