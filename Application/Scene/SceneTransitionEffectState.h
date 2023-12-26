/*
* @file SceneTransitionEffectState.h
* @brief シーン遷移時の演出の状態
*/

#pragma once
#include"PostEffectManager.h"


//前方宣言
class SceneTransitionManager;

//ステート親クラス
class SceneTransitionEffectState
{
protected:
	const float POST_PERA_SIZE_MAX_ = 1.0f;

protected:
	int32_t timer_ = 0;
	SceneTransitionManager* sceneTransitionManager_ = nullptr;
	//読み込み用関数
	std::function<void()>loadFunc_;

	//演出の画面サイズ倍率
	const float WINDOW_SIZE_EXTEND_ = 0.8f;

public:
	//デストラクタ
	virtual ~SceneTransitionEffectState() { ; }

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update();
	//描画
	virtual void Draw() = 0;

	//時間の経過割合
	float GetTimerT(int32_t timer, const int32_t TIMER_MAX) { return min((float)timer / (float)TIMER_MAX, 1.0f); }
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
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
};

//演出はじめ
class TransitionEffectBeginState : public SceneTransitionEffectState
{
private:
	const float POST_NOISE_POW_ = 1.0f;
	const float PLAY_SOUND_VOL_ = 0.7f;
	const float POST_BLOOM_POW_RATE_ = 0.1f;

private:
	const int32_t TIMER_MAX_ = 25;

public:
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
};

//演出中
class TransitionEffectDoingState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
};

//演出終わり
class TransitionEffectEndState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 25;

public:
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw()override;
};