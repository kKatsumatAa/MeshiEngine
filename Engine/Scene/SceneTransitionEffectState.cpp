#include "SceneTransitionEffectState.h"
#include "SceneTransitionManager.h"
#include "Util.h"


void SceneTransitionEffectState::SetSceneTransitionEffect(SceneTransitionManager* sceneTransitionEffect,
	const std::function<void()>& loadFunc)
{
	sceneTransitionManager_ = sceneTransitionEffect;
	loadFunc_ = loadFunc;
}

void SceneTransitionEffectState::Update()
{
	timer_++;
}

//--------------------------------------------------------
//何もしていない
void TransitionEffectNothingState::Initialize()
{
	//ノイズ
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isNoise = false;

	//演出フラグオフ
	sceneTransitionManager_->SetIsDoingEffect(false);
}

void TransitionEffectNothingState::Update()
{
	SceneTransitionEffectState::Update();
}

void TransitionEffectNothingState::Draw()
{
}


//--------------------------------------------------------
//演出始め
void TransitionEffectBeginState::Initialize()
{
	//ノイズ
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isNoise = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isBarrelCurve = true;
}

void TransitionEffectBeginState::Update()
{
	SceneTransitionEffectState::Update();

	//湾曲を徐々につよく
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = EaseOut(GetTimerT(timer_, TIMER_MAX_));
	//一枚目の画面の大きさを徐々に小さく
	PostEffectManager::GetInstance().GetPostEffect1()->SetPera2Extend(LerpVec3(
		{ 1.0f ,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_,0,0 },
		EaseOut(GetTimerT(timer_, TIMER_MAX_))).x_);

	//時間が終わったら
	if (GetIsTimeOver(timer_, TIMER_MAX_))
	{
		//演出ないステートに
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectDoingState>());
	}
}

void TransitionEffectBeginState::Draw()
{
}


//--------------------------------------------------------
//演出中
void TransitionEffectDoingState::Initialize()
{
	//預かっていたロード用の関数を実行
	sceneTransitionManager_->BeginAsyncLoad(loadFunc_);
}

void TransitionEffectDoingState::Update()
{
	SceneTransitionEffectState::Update();

	//ローディング終わったら
	if (!sceneTransitionManager_->GetIsLoadingOnly() && GetIsTimeOver(timer_, TIMER_MAX_))
	{
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectEndState>());
	}
}

void TransitionEffectDoingState::Draw()
{
}


//--------------------------------------------------------
//演出終わり
void TransitionEffectEndState::Initialize()
{
}

void TransitionEffectEndState::Update()
{
	SceneTransitionEffectState::Update();

	//時間が終わったら
	if (GetIsTimeOver(timer_, TIMER_MAX_))
	{
		//演出ないステートに
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectNothingState>());
	}
}

void TransitionEffectEndState::Draw()
{
}