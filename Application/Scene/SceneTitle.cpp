#include "SceneTitle.h"
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
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.barrelCurvePow = POST_BARREL_POW_;
	//ブルーム
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.isBloom = true;
#ifdef _DEBUG
	PostEffectManager::GetInstance().GetPostEffect4()->effectFlags_.isBloom = false;
#endif // _DEBUG

	//海
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isSea = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isSeaImageEffect = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaCol = POST_SEA_COLOR_;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaCameraPos = POST_SEA_CAMERA_POS_;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaSkyCol = POST_SKY_COLOR_;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = POST_SEA_CAMERA_ROT_;

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
		PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("STAGESELECT");
	}
	//タイトルでescapeで終了
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		sceneM_->SetIsEscapingGame(true);
	}
}

void SceneTitle::DrawShadow()
{
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

