/*
* @file StageState.h
* @brief ステージの状態（戦闘時かクリア後か等）
*/

#pragma once
#include"Util.h"


//ステージの状態親クラス
class StageState
{
protected:
	int32_t timer_ = 0;

public:
	virtual ~StageState() { ; }

	virtual void Initialize();
	virtual void Update();
	virtual void DrawShadow();
	virtual void Draw();
	virtual void DrawSprite();
	virtual void DrawImgui();

public:
	static std::unique_ptr<StageState> GetState(const std::string& name);
};


//---------------------------------------------------
//開始時
class StageStateBegining : public StageState
{
protected:
	const float EFFECT_TIMER_MAX_ = 45;

public:
	void Initialize() override;
	void Update() override;
	void DrawShadow() override;
	void Draw() override;
	void DrawSprite() override;
};

//戦闘時
class StageStateBattle : public StageState
{
protected:
	const float CLEAR_ADD_GAME_VEL_ = -1.0f / 60.0f;

	const float CLEAR_COOL_TIMER_ = 60;
	const float GAME_OVER_COOL_TIMER_ = 60;

public:
	void Initialize() override;
	void Update() override;
	void DrawShadow() override;
	void Draw() override;
	void DrawSprite() override;
};


//死亡後
class StageStateDead : public StageState
{
private:
	const float EFFECT_TIMER_MAX_ = 40;

public:
	void Initialize() override;
	void Update() override;
	void DrawShadow() override;
	void Draw() override;
	void DrawSprite() override;
};


//クリア後
class StageStateClear : public StageState
{
private:
	const int32_t CLEAR_COOL_TIME_MAX_ = 20;

public:
	void Initialize() override;
	void Update() override;
	void DrawShadow() override;
	void Draw() override;
	void DrawSprite() override;
};