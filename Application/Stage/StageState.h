#pragma once
#include"Util.h"


//�X�e�[�W�̏�Ԑe�N���X
class StageState
{
protected:
	int32_t timer_ = 0;


public:
	virtual ~StageState() { ; }

	virtual void Initialize();
	virtual void Update();
	virtual void Draw();
	virtual void DrawSprite();
	virtual void DrawImgui();

public:
	static std::unique_ptr<StageState> GetState(const std::string& name);
};


//---------------------------------------------------
//�J�n��
class StageStateBegining : public StageState
{
protected:
	const float EFFECT_TIMER_MAX_ = 75;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawSprite() override;
};

//�퓬��
class StageStateBattle : public StageState
{
protected:
	const float CLEAR_ADD_GAME_VEL_ = -1.0f / 60.0f;

	const float CLEAR_COOL_TIMER_ = 60;
	const float GAME_OVER_COOL_TIMER_ = 60;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawSprite() override;
};


//���S��
class StageStateDead : public StageState
{
private:
	const float EFFECT_TIMER_MAX_ = 40;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawSprite() override;
};


//�N���A��
class StageStateClear : public StageState
{
private:
	const int32_t CLEAR_COOL_TIME_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawSprite() override;
};