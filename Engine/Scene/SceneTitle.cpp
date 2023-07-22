#include "SceneTitle.h"
#include "SceneManager.h"
#include "GameVelocityManager.h"


void SceneTitle::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	sceneM_->StopWaveAllScene();
	ParticleManager::GetInstance()->ClearParticles();

	TextureManager::LoadGraph(L"title.png", titleTex_);

	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isVignette = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = 1.0f;

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect1()->SetPera2Extend(GameVelocityManager::GetInstance().DISPLAY_SIZE_MIN_);
}

void SceneTitle::Update()
{
	//タイトル画面
	if (t_ < 1.0f)
	{
		t_ = (float)titleTime_ / (float)TITLE_TIME_MAX_;

		titleScale_ = LerpVec3({ TITLE_SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x_;

		titleTime_++;
	}

	//シーン遷移
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isScanningLine = false;

		sceneM_->ChangeScene("GAME");
	}
}

void SceneTitle::Draw()
{
	titleObj_.DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		titleScale_, { 1.0f,1.0f,1.0f,1.0f }, titleTex_, { 0.5f,0.5f });
}

void SceneTitle::DrawSprite()
{
}

void SceneTitle::DrawImgui()
{
}

void SceneTitle::DrawPostEffect()
{
}

void SceneTitle::DrawPostEffect2()
{
}

