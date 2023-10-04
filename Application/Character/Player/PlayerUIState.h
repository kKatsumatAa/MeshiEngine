#pragma once
#include"Sprite.h"


class PlayerState;

class PlayerUIState
{
protected:
	//スプライト用
	Sprite reticleSprite_;
	uint64_t reticleTexHandle_;

	//演出用
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


//通常
class PlayerUIStateNomal : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//殴る
class PlayerUIStatePunch : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//銃
class PlayerUIStateGun : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};

//拾う
class PlayerUIStatePickUp : public PlayerUIState
{
private:

public:
	virtual void Initialize();
	virtual void Update();
	virtual void DrawSprite();
};