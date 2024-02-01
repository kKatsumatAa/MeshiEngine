#include "ClearEffectState.h"
#include "ClearEffect.h"

using namespace Util;


void ClearEffectState::SpriteUpdate(const std::string& soundName, const std::string& nextStateName)
{
	timerRatio_ = min((float)timer_ / (float)TIMER_MAX_, TIME_RATE_MAX_);

	//線形補間
	scale_ = Lerp(MAX_SCALE_, NORMAL_SCALE_, EaseIn(timerRatio_));

	alpha_ = timerRatio_;

	timer_ = min(timer_, TIMER_MAX_);

	//制限時間過ぎて、音も終わったら
	if (!Sound::GetInstance().CheckPlayingWave(soundName) && timerRatio_ >= TIME_RATE_MAX_)
	{
		Sound::GetInstance().StopWave(soundName);
		clearEffect_->ChangeState(std::move(GetState(nextStateName)));
	}
	else
	{
		timer_++;
	}
}

void ClearEffectState::SpriteDraw()
{
	//位置などセット
	texSprite_.SetTrans(SPRITE_TRANS_);
	texSprite_.SetScale({ scale_,scale_ ,NORMAL_SCALE_ });
	//描画
	texSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
}

std::unique_ptr<ClearEffectState> ClearEffectState::GetState(const std::string& name)
{
	std::unique_ptr<ClearEffectState>state = nullptr;

	if (name == "HYPER")
	{
		state = std::move(std::make_unique<ClearEffectStateHyper>());
	}
	if (name == "HOT")
	{
		state = std::move(std::make_unique<ClearEffectStateHot>());
	}

	return state;
}

//-----------------------------------------------------
//HYPER
void ClearEffectStateHyper::Initialize()
{
	//ループをオンにしないと止まったかどうかの関数使えない
	Sound::GetInstance().PlayWave(SOUND_NAME_, VOLUME_TMP_);

	texHandle_ = TextureManager::GetInstance().LoadGraph("hyper.dds");

	//セット
	texSprite_.SetTexHandle(texHandle_);
	texSprite_.SetColor(SPRITE_COLOR_);
}

void ClearEffectStateHyper::Update()
{
	SpriteUpdate(SOUND_NAME_, "HOT");
}

void ClearEffectStateHyper::Draw()
{
	SpriteDraw();
}


//------------------------------------------------------
//HOT
void ClearEffectStateHot::Initialize()
{
	//ループをオンにしないと止まったかどうかの関数使えない
	Sound::GetInstance().PlayWave(SOUND_NAME_, VOLUME_TMP);

	texHandle_ = TextureManager::LoadGraph("hot.dds");

	//セット
	texSprite_.SetTexHandle(texHandle_);
	texSprite_.SetColor(SPRITE_COLOR_);
}

void ClearEffectStateHot::Update()
{
	SpriteUpdate(SOUND_NAME_, "HYPER");
}

void ClearEffectStateHot::Draw()
{
	SpriteDraw();
}
