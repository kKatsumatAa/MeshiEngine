#include "ClearEffectState.h"
#include "ClearEffect.h"


void ClearEffectState::SpriteUpdate(const std::string& soundName, const std::string& name)
{
	t_ = min((float)timer_ / (float)TIMER_MAX_, 1.0f);

	scale_ = LerpVec3({ MAX_SCALE_,0,0 }, { 1.0f,0,0 }, EaseIn(t_)).x;

	alpha_ = t_;

	timer_ = min(timer_, TIMER_MAX_);

	//制限時間過ぎて、音も終わったら
	if (!Sound::GetInstance().CheckPlayingWave(soundName) && t_ >= 1.0f)
	{
		Sound::GetInstance().StopWave(soundName);
		clearEffect_->ChangeState(std::move(GetState(name)));
	}
	else
	{
		timer_++;
	}
}

void ClearEffectState::SpriteDraw()
{
	texSprite_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	texSprite_.SetScale({ scale_,scale_ ,1.0f });

	texSprite_.DrawBoxSprite(nullptr, { 0.5f,0.5f });
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

	texHandle_ = TextureManager::GetInstance().LoadGraph("hyper.png");

	//セット
	texSprite_.SetTexHandle(texHandle_);
	texSprite_.SetColor({ 1.0f,1.0f,1.0f,1.0f });
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

	texHandle_ = TextureManager::LoadGraph("hot.png");
}

void ClearEffectStateHot::Update()
{
	SpriteUpdate(SOUND_NAME_, "HYPER");
}

void ClearEffectStateHot::Draw()
{
	SpriteDraw();
}
