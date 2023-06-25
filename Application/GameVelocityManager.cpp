#include "GameVelocityManager.h"
#include "Util.h"
#include "ImGuiManager.h"

GameVelocityManager& GameVelocityManager::GetInstance()
{
	static GameVelocityManager sInst;
	return sInst;
}

void GameVelocityManager::Initialize()
{
	gameVelocity_ = 0;
}

void GameVelocityManager::AddGameVelocity(float velocity, std::string velName)
{
	gameVelocity_ += velocity * 3.0f;
	//�ő�X�s�[�h�����Ȃ��悤��
	if (velName == "mouse")
	{
		//�}�E�X��
		gameVelocity_ = min(GAME_VELOCITY_MAX_ / 7.0f, gameVelocity_);
	}
	else
	{
		gameVelocity_ = min(GAME_VELOCITY_MAX_, gameVelocity_);
	}
}

float GameVelocityManager::GetVelocity()
{
	return gameVelocity_;
}

void GameVelocityManager::Update()
{
	//���炵�Ă���
	gameVelocity_ *= 0.95f;
	//�Œ�X�s�[�h�͕ۂ�
	gameVelocity_ = max(GAME_VELOCITY_MIN_, gameVelocity_);
}


void GameVelocityManager::UpdateImGui()
{
	ImGui::Begin("GameSpeed");
	ImGui::SliderFloat("gameVelocity", &gameVelocity_, GAME_VELOCITY_MIN_, GAME_VELOCITY_MAX_);
	ImGui::End();
}