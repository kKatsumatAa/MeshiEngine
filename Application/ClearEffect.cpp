#include "ClearEffect.h"
#include "ClearEffectState.h"
#include "LevelManager.h"

ClearEffect& ClearEffect::GetInstance()
{
	static ClearEffect inst;
	return inst;
}

void ClearEffect::ChangeState(std::unique_ptr<ClearEffectState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetClearEffect(this);
	state_->Initialize();
}

void ClearEffect::BeginClearEffect()
{
	if (!isEffect_)
	{
		isEffect_ = true;
		//ステート変更
		ChangeState(ClearEffectState::GetState("HYPER"));
	}
}

void ClearEffect::Initialize()
{
	Sound::GetInstance().LoadWave("hyper.wav", false);
	Sound::GetInstance().LoadWave("hot.wav", false);

	if (state_)
	{
		state_.reset();
		state_ = nullptr;
	}

	isEffect_ = false;

	TextureManager::LoadGraph(L"hyper.png");
	TextureManager::LoadGraph(L"hot.png");
}

void ClearEffect::Update()
{
	//クリア演出開始
	if (!GetIsEffect() && LevelManager::GetInstance().GetGameClear())
	{
		BeginClearEffect();
	}

	state_->Update();
}

void ClearEffect::Draw()
{
	state_->Draw();
}
