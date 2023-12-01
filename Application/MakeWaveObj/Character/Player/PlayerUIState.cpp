#include "PlayerUIState.h"
#include "PlayerUI.h"


std::unique_ptr<PlayerUIState> PlayerUIState::GetState(const std::string& name)
{
	std::unique_ptr<PlayerUIState> state;

	if (name == "NORMAL")
	{
		state = std::make_unique<PlayerUIStateNomal>();
	}
	if (name == "PUNCH")
	{
		state = std::make_unique<PlayerUIStatePunch>();
	}
	if (name == "GUN")
	{
		state = std::make_unique<PlayerUIStateGun>();
	}
	if (name == "PICKUP")
	{
		state = std::make_unique<PlayerUIStatePickUp>();
	}

	return std::move(state);
}

//-------------------------------------------------------------
void PlayerUIState::Initialize()
{
	playerUI_->SetScale(SCALE_MAX_);

	reticleSprite_.SetColor(color_);
}

void PlayerUIState::Update()
{
	float t = min((float)timer_ / (float)TIMER_MAX_, 1.0f);

	playerUI_->SetScale(LerpVec3({ SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseInOutBack(t)).x);

	timer_++;
}

void PlayerUIState::DrawSprite()
{
	reticleSprite_.SetTrans({ playerUI_->GetPos().x,playerUI_->GetPos().y,0 });
	reticleSprite_.SetScale({ playerUI_->GetScale() * playerUI_->GetScale2(), playerUI_->GetScale() * playerUI_->GetScale2() ,1.0f });
	reticleSprite_.SetRot({ 0,0 ,angle_ });

	reticleSprite_.DrawBoxSprite(nullptr, { 0.5f,0.5f });
}


//------------------------------------------------------------------------------------------
//通常
void PlayerUIStateNomal::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("normalReticle.png");
	reticleSprite_.SetTexHandle(reticleTexHandle_);
}

void PlayerUIStateNomal::Update()
{
	PlayerUIState::Update();
}

void PlayerUIStateNomal::DrawSprite()
{
	PlayerUIState::DrawSprite();
}


//------------------------------------------------------------------------------------------
//殴れる
void PlayerUIStatePunch::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("punchReticle.png");
	reticleSprite_.SetTexHandle(reticleTexHandle_);
}

void PlayerUIStatePunch::Update()
{
	PlayerUIState::Update();
}

void PlayerUIStatePunch::DrawSprite()
{
	PlayerUIState::DrawSprite();
}


//------------------------------------------------------------------------------------------
//銃
void PlayerUIStateGun::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("gunReticle.png");
	reticleSprite_.SetTexHandle(reticleTexHandle_);
}

void PlayerUIStateGun::Update()
{
	PlayerUIState::Update();
}

void PlayerUIStateGun::DrawSprite()
{
	PlayerUIState::DrawSprite();
}

//------------------------------------------------------------------------------------------
//拾える
void PlayerUIStatePickUp::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("pickUpReticle.png");
	reticleSprite_.SetTexHandle(reticleTexHandle_);
}

void PlayerUIStatePickUp::Update()
{
	PlayerUIState::Update();
}

void PlayerUIStatePickUp::DrawSprite()
{
	PlayerUIState::DrawSprite();
}
