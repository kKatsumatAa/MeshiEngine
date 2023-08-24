#include "SceneGame.h"
#include "SceneManager.h"
#include "StageManager.h"
#include "StageState.h"
#include "StageSelect.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//ステージ読み込み
	StageManager::GetInstance().LoadStage(StageSelect::GetInstance().GetSelectStageIndex());

	//
	ClearEffect::GetInstance().Initialize();

	ParticleManager::GetInstance()->SetBlendNum(ParticleManager::TRIANGLE);

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isVignette = true;

	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isDepthField = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.focusDepth = 0.797f;//0.742
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.nFocusWidth = 0.862f;//1.316
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.focusDiffPow = 1.272f;//1.142
}

void SceneGame::Update()
{
	//リセット
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		sceneM_->SetNextScene("GAME");
	}

	//ステージアップデート
	StageManager::GetInstance().Update();

	//シーン遷移
	if (StageManager::GetInstance().GetIsGameOver())
	{
		sceneM_->SetNextScene("GAME");
	}
	else if (StageManager::GetInstance().GetIsGameClear() || KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		ParticleManager::GetInstance()->ClearParticles();
		sceneM_->SetNextScene("STAGESELECT");
	}
}

void SceneGame::Draw()
{
	StageManager::GetInstance().Draw();
}

void SceneGame::DrawSprite()
{
	StageManager::GetInstance().DrawSprite();
}

void SceneGame::DrawImgui()
{
	StageManager::GetInstance().DrawImGui();
}
