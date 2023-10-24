#include "SceneStageSelect.h"
#include "SceneManager.h"
#include "GameVelocityManager.h"
#include "CursorUI.h"
#include "StageSelect.h"


void SceneStageSelect::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneStageSelect::Initialize()
{
	ObjectManager::GetInstance().ClearAllObj();
	ParticleManager::GetInstance()->ClearParticles();

	sceneM_->StopWaveAllScene();

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = 0.7f;

	//カーソルui
	CursorUI::GetInstance().Initialize();
	//ステージセレクト
	StageSelect::GetInstance().Initialize();
	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();
}

void SceneStageSelect::Update()
{
	//カーソルui
	CursorUI::GetInstance().Update();
	//セレクト
	StageSelect::GetInstance().Update();

	//シーン遷移
	if (StageSelect::GetInstance().GetIsSelected())
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("GAME");
	}
	//タイトルでescapeで終了
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		sceneM_->SetNextScene("TITLE");
	}
}

void SceneStageSelect::Draw()
{

}

void SceneStageSelect::DrawSprite()
{
	//セレクト
	StageSelect::GetInstance().DrawSprite();

	//カーソルui
	CursorUI::GetInstance().Draw();
}

void SceneStageSelect::DrawImgui()
{
}

