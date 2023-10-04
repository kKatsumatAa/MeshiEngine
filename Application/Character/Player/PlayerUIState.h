#pragma once
#include"Sprite.h"


class PlayerState;

class PlayerUIState
{
protected:
	//�X�v���C�g�p
	Sprite reticleSprite_;
	uint64_t reticleTexHandle_;

	//���o�p
	float timer_ = 0;
	const float TIMER_MAX_ = 10;
	float scale_ = 1.0f;
	const float SCALE_MAX_ = 1.5f;
	Vec4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	float angle_ = 0;

public:
	static std::unique_ptr<PlayerUIState> GetState(const std::string& name);

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();

public:
	virtual void SetColor(const Vec4& color) { color_ = color; }
	virtual void SetAngle(float angle) { angle_ = angle; }
};


//�ʏ�
class PlayerUIStateNomal : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//����
class PlayerUIStatePunch : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//�e
class PlayerUIStateGun : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//�E��
class PlayerUIStatePickUp : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};