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
	gameVelocity_ = GAME_VELOCITY_MAX_;
	isInvalidAddGameVel_ = false;
	isNormalTime_ = false;

	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isRadialBlur = false;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.color = VEL_COLOR_MAX_;
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
	//スピードで徐々に色変える
	Vec3 col = LerpVec3({ VEL_COLOR_MAX_.x,VEL_COLOR_MAX_.y,VEL_COLOR_MAX_.z },
		VEL_COLOR_MIN_, EaseInOut(1.0f - gameVelocity_));
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.color = { col.x,col.y,col.z,1.0f };

	//（三つ目に描画されたものを）スピードでラジアルブラーの強さ変える
	if ((1.0f - gameVelocity_ / GAME_VELOCITY_MAX_) * VEL_RADIAL_BLUR_RATE_ >= 0)
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isRadialBlur = true;
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.radialPow = (1.0f - gameVelocity_ / GAME_VELOCITY_MAX_) * VEL_RADIAL_BLUR_RATE_;
	}
	else
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isRadialBlur = false;
	}

	if (gameVelocity_ > GAME_VELOCITY_MIN_ && !isNormalTime_)
	{
		//減らしていく
		gameVelocity_ *= GAME_VEL_DECTREMENT_;
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