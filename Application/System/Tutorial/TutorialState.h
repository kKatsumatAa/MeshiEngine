/*
* @file TutorialState.h
* @brief チュートリアルの状態
*/

#pragma once
#include"Sprite.h"

class Tutorial;


class TutorialState
{
public:
	const Vec3 SPRITE_POS_ = { WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 };
	const Vec3 SPRITE_COLOR_ = { 2.0f,2.0f,2.0f };
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };
	const float TIMER_RATE_MAX_ = 1.0f;
	const float MAX_SCALE_ = 1.5f;
	const float NORMAL_SCALE_ = 1.0f;
	//演出の時間制限
	const int32_t BEGINING_TIMER_MAX_ = 10;
	const int32_t ENDING_TIMER_MAX_ = 10;

protected:
	Tutorial* tutorial_;

	//条件達成したか
	bool isAchievement_ = false;

	//演出のタイマー
	int32_t endingTimer_ = 0;
	int32_t beginingTimer_ = 0;

	float t_ = 0;

	float scale_ = 0;

	float alpha_ = 0.0f;

	//画像
	uint64_t texHandle_ = NULL;
	Sprite texSprite_;

	//条件達成率
	float rate_ = 0;
	int32_t rateMax_ = 150;


public:
	//デストラクタ
	virtual ~TutorialState() { ; }

public:
	//チュートリアルのポインタセット
	void SetTutorial(Tutorial* tutorial) { tutorial_ = tutorial; }

	//名前でステートを取得
	static std::unique_ptr<TutorialState>GetState(const std::string& name);

public:
	//共通の更新
	virtual void CommonUpdate();
	//共通のスプライト描画
	virtual void CommonSpriteDraw();

	//条件達成後の演出
	virtual bool EndingUpdate(const int32_t TIMER_MAX);
	//最初の演出処理
	virtual bool BeginingUpdate(const int32_t TIMER_MAX);

public:
	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
};


//何もない
class TutorialStateNone : public TutorialState
{
public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//基本操作説明
class TutorialStateOperation : public TutorialState
{
public:
	const int32_t RATE_MAX_TMP_ = 50;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//ゲームスピードの説明
class TutorialStateGameSpeed : public TutorialState
{
public:
	const int32_t RATE_MAX_TMP_ = 30;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//マウス操作説明
class TutorialStateMouse : public TutorialState
{
public:
	const int32_t RATE_MAX_TMP_ = 100;

private:
	bool isPushedLeftClick = false;
	bool isPushedRightClick = false;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};