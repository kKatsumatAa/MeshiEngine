#include "PlayerUIState.h"
#include "PlayerUI.h"

using namespace Util;


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
	float t = min((float)timer_ / (float)TIMER_MAX_, TIME_RATIO_MAX_);

	playerUI_->SetScale(Lerp(SCALE_MAX_, SCALE_NORMAL_, EaseInOutBack(t)));

	timer_++;
}

void PlayerUIState::DrawSprite()
{
	reticleSprite_.SetTrans({ playerUI_->GetPos().x,playerUI_->GetPos().y,0 });
	reticleSprite_.SetScale({ playerUI_->GetScale() * playerUI_->GetScaleRatio(), playerUI_->GetScale() * playerUI_->GetScaleRatio() ,SCALE_NORMAL_ });
	reticleSprite_.SetRot({ 0,0 ,angle_ });

	reticleSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
}


//------------------------------------------------------------------------------------------
//通常
void PlayerUIStateNomal::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("normalReticle.dds");
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
	reticleTexHandle_ = TextureManager::LoadGraph("punchReticle.dds");
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
	reticleTexHandle_ = TextureManager::LoadGraph("gunReticle.dds");
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
	reticleTexHandle_ = TextureManager::LoadGraph("pickUpReticle.dds");
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
