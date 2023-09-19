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
	if (velocity == 0 || isInvalidAddGameVel_)
	{
		return;
	}


	//最大スピード超えないように
	if (velName == "mouse" && gameVelocity_ >= MOUSE_VEL_MAX_)
	{
		velocity = 0;
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
	//（三つ目に描画されたものを）スピードで湾曲の強さ変える
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.barrelCurvePow = (1.0f - gameVelocity_ / GAME_VELOCITY_MAX_) * 0.6f;
	//（二つ目の）画面の大きさも
	PostEffectManager::GetInstance().GetPostEffect2()->SetPera2Extend(
		LerpVec3({ PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ ,0,0 }, { 1.0f,0,0 },
			(gameVelocity_ / GAME_VELOCITY_MAX_)).x);
	//三つ目の色も
	Vec3 col = LerpVec3(VEL_COL_MIN_, { 1.0f,1.0f,1.0f }, gameVelocity_);
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.color = { col.x,col.y,col.z,1.0f };

	if (gameVelocity_ > GAME_VELOCITY_MIN_)
	{
		//減らしていく
		gameVelocity_ *= 0.83f;
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