#include "SceneGame.h"
#include "SceneManager.h"
#include "StageManager.h"
#include "StageState.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//ステージ読み込み
	StageManager::GetInstance().LoadStage(1);

	//
	ClearEffect::GetInstance().Initialize();

	ParticleManager::GetInstance()->SetBlendNum(ParticleManager::TRIANGLE);

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isVignette = true;
}

void SceneGame::Update()
{

	//#ifdef _DEBUG
		//リセット
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		sceneM_->SetNextScene("GAME");
	}
	//#endif

		//ステージアップデート
	StageManager::GetInstance().Update();

	//シーン遷移
	if (StageManager::GetInstance().GetIsGameOver())
	{
		sceneM_->SetNextScene("TITLE");
	}
	else if (StageManager::GetInstance().GetIsGameClear())
	{
		ParticleManager::GetInstance()->ClearParticles();
		sceneM_->SetNextScene("TITLE");
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
