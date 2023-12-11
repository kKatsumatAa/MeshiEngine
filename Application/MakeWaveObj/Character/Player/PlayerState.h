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
	//共通の移動などの処理
	std::function<void()>commonUpdateF_;

public:
	virtual ~PlayerState() { ; }

	void SetPlayer(Player* player) { player_ = player; }
	void SetCommonUpdateF(const std::function<void()>& f) { commonUpdateF_ = f; }

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
class PlayerStateDeadEffect :
	public PlayerState,
	public CharacterStateDead
{
private:
	Vec3 targetPos_;
	Vec3 dir_;
	const float LENGTH_MIN_ = 5.0f;

public:
	void Initialize() override;
	void Update() override;
};

//死亡演出2(マグマで)
class PlayerStateDeadEffect2 :
	public PlayerState,
	public CharacterStateDead
{
private:
	Vec3 beginRot_ = Vec3(0, 0, 0);
	const float endRotX_ = -PI / 2.0f;

public:
	void Initialize() override;
	void Update() override;
};

//死亡演出3(パンチで)
class PlayerStateDeadEffectPunched :
	public PlayerState,
	public CharacterStateDead
{
private:
	Vec3 beginRot_ = Vec3(0, 0, 0);
	const float endRotX_ = -PI / 2.0f;
	Vec3 beginPos_ = Vec3(0, 0, 0);
	const float LENGTH_MAX_ = 5.0f;
	Vec3 dir_;

public:
	void Initialize() override;
	void Update() override;
};