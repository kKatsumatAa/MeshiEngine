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
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.barrelCurvePow = POST_BARREL_POW_;

	//カーソルui
	CursorUI::GetInstance().Initialize();
	//ステージセレクト
	StageSelect::GetInstance().Initialize();
	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();

	//海
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaCameraPos = POST_SEA_CAMERA_POS_;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = POST_SEA_CAMERA_ROT_;
	//画面の色
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.color = POST_SCREEN_COLOR_;

	//モデル解放
	ModelManager::GetInstance().ClearModels();
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
		PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("GAME");
	}
	//タイトルでescapeで終了
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		sceneM_->SetNextScene("TITLE");
	}
}

void SceneStageSelect::DrawShadow()
{
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

