#pragma once
#include"Object.h"

class Tutorial;


class TutorialState
{
protected:
	Tutorial* tutorial_;

	//条件達成したか
	bool isAchievement_ = false;

	//演出のタイマー
	int32_t endingTimer_ = 0;
	int32_t beginingTimer_ = 0;

	float t_ = 0;

	float scale_ = 0;
	const float MAX_SCALE_ = 1.5f;

	float alpha_ = 0.0f;

	//画像
	uint64_t texHandle_ = NULL;
	Object texObj_;

	//演出の時間制限
	const int32_t BEGINING_TIMER_MAX_ = 10;
	const int32_t ENDING_TIMER_MAX_ = 10;

	//条件達成率
	float rate_ = 0;
	int32_t rateMax_ = 150;


public:
	virtual ~TutorialState() { ; }

public:
	void SetTutorial(Tutorial* tutorial) { tutorial_ = tutorial; }

	static std::unique_ptr<TutorialState>GetState(const std::string& name);

	virtual void CommonUpdate();
	virtual void CommonSpriteDraw();

	//条件達成後の演出
	virtual bool EndingUpdate(const int32_t TIMER_MAX);
	//最初の演出処理
	virtual bool BeginingUpdate(const int32_t TIMER_MAX);

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};


//何もない
class TutorialStateNone : public TutorialState
{
private:

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//基本操作説明
class TutorialStateOperation : public TutorialState
{
private:


public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//ゲームスピードの説明
class TutorialStateGameSpeed : public TutorialState
{
private:
	

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//マウス操作説明
class TutorialStateMouse : public TutorialState
{
private:
	bool isPushedLeftClick = false;
	bool isPushedRightClick = false;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};