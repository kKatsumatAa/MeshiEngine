#include "GameVelocityManager.h"
#include "Util.h"
#include "ImGuiManager.h"
#include "PostEffectManager.h"


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
	if (velocity == 0)
	{
		return;
	}


	//最大スピード超えないように
	if (velName == "mouse")
	{
		velocity = (GAME_VELOCITY_MAX_ / 7.0f - gameVelocity_) * 0.1f;
	}

	gameVelocity_ += velocity;
	gameVelocity_ = min(gameVelocity_, GAME_VELOCITY_MAX_);
	gameVelocity_ = max(gameVelocity_, GAME_VELOCITY_MIN_);
}

float GameVelocityManager::GetVelocity()
{
	return gameVelocity_;
}

void GameVelocityManager::Update()
{
	//スピードで湾曲の強さ変える
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = (1.0f - gameVelocity_ / GAME_VELOCITY_MAX_) *0.6f;
	//画面の大きさも
	PostEffectManager::GetInstance().GetPostEffect1()->SetPera2Extend(LerpVec3({ DISPLAY_SIZE_MIN_ ,0,0 }, { 1.0f,0,0 },
		(gameVelocity_ / GAME_VELOCITY_MAX_)).x_);

	if (gameVelocity_ > GAME_VELOCITY_MIN_)
	{
		//減らしていく
		gameVelocity_ *= 0.8f;
		//最低スピードは保つ
		gameVelocity_ = max(GAME_VELOCITY_MIN_, gameVelocity_);
	}
}


void GameVelocityManager::UpdateImGui()
{
	ImGui::Begin("GameSpeed");
	ImGui::SliderFloat("gameVelocity", &gameVelocity_, GAME_VELOCITY_MIN_, GAME_VELOCITY_MAX_);
	ImGui::End();
}