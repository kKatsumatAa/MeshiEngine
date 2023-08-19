#pragma once
#include"PostEffectManager.h"


//前方宣言
class SceneTransitionManager;

//ステート親クラス
class SceneTransitionEffectState
{
protected:
	int32_t timer_ = 0;
	SceneTransitionManager* sceneTransitionManager_ = nullptr;
	//読み込み用関数
	std::function<void()>loadFunc_;

	//演出の画面サイズ倍率
	const float WINDOW_SIZE_EXTEND_ = 0.9f;


public:
	virtual ~SceneTransitionEffectState() { ; }

	virtual void Initialize() = 0;
	virtual void Update();
	virtual void Draw() = 0;

	//時間の経過割合
	float GetTimerT(int32_t timer, const int32_t TIMER_MAX) { return (float)timer / (float)TIMER_MAX; }
	//時間がオーバーしたか
	bool GetIsTimeOver(int32_t timer, const int32_t TIMER_MAX) { return GetTimerT(timer, TIMER_MAX) >= 1.0f; }

	//ステートの持ち主のポインタと読み込み用関数をもらう
	void SetSceneTransitionEffect(SceneTransitionManager* sceneTransitionEffect,
		const std::function<void()>& loadFunc);
};


//演出していない
class TransitionEffectNothingState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 30;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//演出はじめ
class TransitionEffectBeginState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//演出中
class TransitionEffectDoingState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//演出終わり
class TransitionEffectEndState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};