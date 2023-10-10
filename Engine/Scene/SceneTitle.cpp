﻿#include "SceneTitle.h"
#include "SceneManager.h"
#include "GameVelocityManager.h"
#include "TitleUI.h"
#include "CursorUI.h"


void SceneTitle::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	ObjectManager::GetInstance().ClearAllObj();
	ParticleManager::GetInstance()->ClearParticles();

	sceneM_->StopWaveAllScene();

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = 0.7f;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isDepthField = false;
	//ブルーム
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = true;
#ifdef _DEBUG
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = false;
#endif // _DEBUG


	//カーソルui
	CursorUI::GetInstance().Initialize();
	//タイトル
	TitleUI::GetInstance().Initialize();
	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();
}

void SceneTitle::Update()
{
	//カーソルui
	CursorUI::GetInstance().Update();
	//タイトル
	TitleUI::GetInstance().Update();

	//シーン遷移
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("STAGESELECT");
	}
	//タイトルでescapeで終了
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		sceneM_->SetIsEscapingGame(true);
	}
}

void SceneTitle::Draw()
{

}

void SceneTitle::DrawSprite()
{
	//タイトル
	TitleUI::GetInstance().DrawSprite();

	//カーソルui
	CursorUI::GetInstance().Draw();
}

void SceneTitle::DrawImgui()
{
}

