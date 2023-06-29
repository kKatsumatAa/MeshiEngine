#pragma once
#include"Util.h"

class Player;

class PlayerAttackState
{
protected:
	//��Ԃ��g���C���X�^���X�|�C���^
	Player* player_ = nullptr;
	int32_t timer_ = 0;

public:
	void SetPlayer(Player* player) { player_ = player; }

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

//�U�����Ă��Ȃ�
class PlayerAttackStateNone : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//�U����
class PlayerAttackStateDoing : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//�U�����Q
class PlayerAttackStateDoing2 : public PlayerAttackState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};