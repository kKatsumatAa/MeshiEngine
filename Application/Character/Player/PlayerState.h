#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"


class Player;


//素手か、銃を持ってるか等のステート
class PlayerState : public CharacterState
{
protected:
	const float PARTICLE_SIZE_EXTEND_ = 0.2f;

protected:
	Player* player_ = nullptr;

public:
	virtual ~PlayerState() { ; }

	void SetPlayer(Player* player) { player_ = player; }

	bool CheckEyeRayHit()override;

	virtual void Initialize()override = 0;
	virtual void Update()override;
};

//素手状態
class PlayerStateBareHands :
	public PlayerState,
	public CharacterStateBareHands

{

public:
	void Initialize() override;
	void Update() override;
};

//銃持ってる
class PlayerStateHaveWeapon :
	public PlayerState,
	public CharacterStateHaveWeapon
{

public:
	void Initialize() override;
	void Update() override;
};

//死亡演出
class PlayerStateDeadEffect : public PlayerState
{
private:
	const int32_t TIMER_MAX_ = 70;
	Vec3 targetPos_;
	Vec3 dir_;
	const float LENGTH_MIN_ = 5.0f;

public:
	void Initialize() override;
	void Update() override;
};
