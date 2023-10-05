#pragma once
#include"Util.h"
#include<functional>


class PlayerHand;

class PlayerAttackState
{
protected:
	//ó‘Ô‚ğg‚¤ƒCƒ“ƒXƒ^ƒ“ƒXƒ|ƒCƒ“ƒ^
	PlayerHand* playerHand_ = nullptr;
	float timer_ = 0;

	//‰ñ“]‚Ég‚¤Šî€’l
	static Vec3 rotTmp_;

	//“G‚Ì”í’eˆ—
	std::function<void(PlayerHand*)> enemyDamageFunc_ = nullptr;

public:
	virtual ~PlayerAttackState();

	void SetEnemyDamageFunc(std::function<void(PlayerHand*)> f) { enemyDamageFunc_ = f; }

	void SetPlayerHand(PlayerHand* playerHand) { playerHand_ = playerHand; }

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

//UŒ‚‚µ‚Ä‚¢‚È‚¢
class PlayerAttackStateNone : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//UŒ‚’†
class PlayerAttackStateDoing : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 2;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//UŒ‚’†‚Q
class PlayerAttackStateDoing2 : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 7;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};