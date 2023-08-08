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
	scale_ = SCALE_MAX_;
}

void PlayerUIState::Update()
{
	float t = min((float)timer_ / (float)TIMER_MAX_, 1.0f);

	scale_ = LerpVec3({ SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseInOutBack(t)).x_;

	timer_++;
}

void PlayerUIState::DrawSprite()
{
	reticleObj_.DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		scale_, color_, reticleTexHandle_, { 0.5f,0.5f }, false, false, angle_);
}


//------------------------------------------------------------------------------------------
//�ʏ�
void PlayerUIStateNomal::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("normalReticle.png");
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
//�����
void PlayerUIStatePunch::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("punchReticle.png");
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
//�e
void PlayerUIStateGun::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("gunReticle.png");
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
//�E����
void PlayerUIStatePickUp::Initialize()
{
	reticleTexHandle_ = TextureManager::LoadGraph("pickUpReticle.png");
}

void PlayerUIStatePickUp::Update()
{
	PlayerUIState::Update();
}

void PlayerUIStatePickUp::DrawSprite()
{
	PlayerUIState::DrawSprite();
}
