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
		//�X�e�[�g�ύX
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
	//�N���A���o�J�n
	if (!GetIsEffect() && 
		ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_ENEMYS).size() <= 0)
	{
		BeginClearEffect();
	}

	state_->Update();
}

void ClearEffect::Draw()
{
	state_->Draw();
}
