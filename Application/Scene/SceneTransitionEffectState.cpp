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
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isNoise = false;

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
	Sound::GetInstance().PlayWave(SceneTransitionManager::TRANSITION_SOUND_NAME_, PLAY_SOUND_VOL_);

	//二つ目ノイズ
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isNoise = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.noisePow = POST_NOISE_POW_;
	//三つ目に描画されたもの（２枚目まで）を湾曲
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.isBarrelCurve = true;

	//ブルーム
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.bloomPow = PostPera::S_BLOOM_MAX_POW_ * POST_BLOOM_POW_RATE_;

#ifdef _DEBUG

	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.isBloom = false;

#endif // debug
}

void TransitionEffectBeginState::Update()
{
	SceneTransitionEffectState::Update();

	//湾曲を徐々につよく
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.barrelCurvePow = EaseInOutBack(GetTimerT(timer_, TIMER_MAX_));
	//2枚目の画面の大きさを徐々に小さく
	PostEffectManager::GetInstance().GetPostEffect3()->SetPera2Extend(Lerp(
		POST_PERA_SIZE_MAX_, PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_,
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))));

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

	//湾曲を徐々に弱く
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.barrelCurvePow = 1.0f - EaseIn(GetTimerT(timer_, TIMER_MAX_ / 2));
	//2つ目の2枚目の画面の大きさを徐々に大きく
	PostEffectManager::GetInstance().GetPostEffect3()->SetPera2Extend(Lerp(
		PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_, POST_PERA_SIZE_MAX_,
		EaseIn(GetTimerT(timer_, TIMER_MAX_ / 2))));

	//ノイズを徐々に
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.noisePow = 1.0f - EaseIn(GetTimerT(timer_, TIMER_MAX_));

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