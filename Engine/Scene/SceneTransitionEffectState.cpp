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
	//音止める
	Sound::GetInstance().StopWave(SceneTransitionManager::TRANSITION_SOUND_NAME_);

	//２つ目をノイズ
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isNoise = false;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = false;

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
	//音流す
	Sound::GetInstance().PlayWave(SceneTransitionManager::TRANSITION_SOUND_NAME_, true);

	//二つ目ノイズ
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isNoise = true;
	//三つ目に描画されたもの（２枚目まで）を湾曲
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;

	//ブルーム
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = PostPera::S_BLOOM_POW_;

#ifdef _DEBUG

	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = false;

#endif // debug
}

void TransitionEffectBeginState::Update()
{
	SceneTransitionEffectState::Update();

	//湾曲を徐々につよく
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.barrelCurvePow = EaseInOutBack(GetTimerT(timer_, TIMER_MAX_));
	//2枚目の画面の大きさを徐々に小さく
	PostEffectManager::GetInstance().GetPostEffect2()->SetPera2Extend(LerpVec3(
		{ 1.0f ,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_);

	//ブルーム徐々に
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = LerpVec3(
		{ 0.0f ,0,0 }, { PostPera::S_BLOOM_POW_,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_;

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

	//2つ目の2枚目の画面の大きさを徐々に大きく
	PostEffectManager::GetInstance().GetPostEffect2()->SetPera2Extend(LerpVec3(
		{ PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ ,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_);

	//ブルーム徐々に
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = LerpVec3(
		 { PostPera::S_BLOOM_POW_,0,0 }, { 0.0f ,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_;

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