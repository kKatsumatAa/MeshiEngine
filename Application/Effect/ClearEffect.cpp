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

	TextureManager::LoadGraph("hyper.png");
	TextureManager::LoadGraph("hot.png");
}

void ClearEffect::Update()
{
	//クリア演出開始
	if (!GetIsEffect() && 
		ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_ENEMYS).size() <= NUM_ENEMY_OF_CLEAR_)
	{
		BeginClearEffect();
	}

	state_->Update();
}

void ClearEffect::Draw()
{
	state_->Draw();
}
