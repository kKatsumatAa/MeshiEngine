#pragma once
#include"Util.h"
#include<functional>


class PlayerHand;

class PlayerAttackState
{
protected:
	//状態を使うインスタンスポインタ
	PlayerHand* playerHand_ = nullptr;
	float timer_ = 0;

	//回転に使う基準値
	static Vec3 rotTmp_;

	//敵の被弾処理
	std::function<void(PlayerHand*)> enemyDamageFunc_ = nullptr;

public:
	virtual ~PlayerAttackState();

	void SetEnemyDamageFunc(std::function<void(PlayerHand*)> f) { enemyDamageFunc_ = f; }

	void SetPlayerHand(PlayerHand* playerHand) { playerHand_ = playerHand; }

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

//攻撃していない
class PlayerAttackStateNone : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//攻撃中
class PlayerAttackStateDoing : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 2;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//攻撃中２
class PlayerAttackStateDoing2 : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 7;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};