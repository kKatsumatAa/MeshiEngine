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

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isDepthField = false;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isBloom = true;

	//�J�[�\��ui
	CursorUI::GetInstance().Initialize();
	//�^�C�g��
	TitleUI::GetInstance().Initialize();
}

void SceneTitle::Update()
{
	//�J�[�\��ui
	CursorUI::GetInstance().Update();
	//�^�C�g��
	TitleUI::GetInstance().Update();

	//�V�[���J��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("STAGESELECT");
	}
	//�^�C�g����escape�ŏI��
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
	//�^�C�g��
	TitleUI::GetInstance().DrawSprite();

	//�J�[�\��ui
	CursorUI::GetInstance().Draw();
}

void SceneTitle::DrawImgui()
{
}

