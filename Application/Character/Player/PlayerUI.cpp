#include "PlayerUI.h"
#include "PlayerUIState.h"



PlayerUI& PlayerUI::GetInstance()
{
	static PlayerUI sInst;
	return sInst;
}

void PlayerUI::ChangeState(const std::string& name)
{
	if (oldStateName == name)
	{
		return;
	}

	state_.reset();
	state_ = std::move(PlayerUIState::GetState(name));
	state_->SetPlayerUI(this);
	state_->Initialize();

	oldStateName = name;
}

void PlayerUI::Initialize()
{
	TextureManager::LoadGraph("normalReticle.png");
	TextureManager::LoadGraph("punchReticle.png");
	TextureManager::LoadGraph("gunReticle.png");
	TextureManager::LoadGraph("pickUpReticle.png");

	pos_ = { WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f, WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f };

	ChangeState("NORMAL");
}

void PlayerUI::Update()
{
	state_->Update();
}

void PlayerUI::DrawSprite()
{
	state_->DrawSprite();
}

void PlayerUI::SetAngle(float angle)
{
	state_->SetAngle(angle);
}
