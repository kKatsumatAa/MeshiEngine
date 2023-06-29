#pragma once
#include"Util.h"

class Player;

class PlayerAttackState
{
protected:
	//状態を使うインスタンスポインタ
	Player* player_ = nullptr;
	int32_t timer_ = 0;

public:
	void SetPlayer(Player* player) { player_ = player; }

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
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//攻撃中２
class PlayerAttackStateDoing2 : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};