/*
* @file PlayerState.h
* @brief プレイヤー自体の状態（素手か、銃持ってるか、死亡演出中かなど）
*/

#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"

//プレイヤーの前方宣言
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
	//デストラクタ
	virtual ~PlayerState() { ; }

	//プレイヤーポインタセット
	void SetPlayer(Player* player) { player_ = player; }
	//共通処理関数のセット
	void SetCommonUpdateF(const std::function<void()>& f) { commonUpdateF_ = f; }
	//正面ベクトルに何かあった時の処理
	bool CheckEyeRayHit()override;

	//初期化
	virtual void Initialize()override = 0;
	//更新
	virtual void Update()override;
};

//素手状態
class PlayerStateBareHands :
	public PlayerState,
	public CharacterStateBareHands

{
public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//銃持ってる
class PlayerStateHaveWeapon :
	public PlayerState,
	public CharacterStateHaveWeapon
{
public:
	//初期化
	void Initialize() override;
	//更新
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
	//初期化
	void Initialize() override;
	//更新
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
	//初期化
	void Initialize() override;
	//更新
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
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};