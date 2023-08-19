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

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isDepthField = false;

	//�J�[�\��ui
	CursorUI::GetInstance().Initialize();
	//�X�e�[�W�Z���N�g
	StageSelect::GetInstance().Initialize();
}

void SceneStageSelect::Update()
{
	//�J�[�\��ui
	CursorUI::GetInstance().Update();
	//�Z���N�g
	StageSelect::GetInstance().Update();

	//�V�[���J��
	if (StageSelect::GetInstance().GetIsSelected())
	{
		PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isScanningLine = false;

		sceneM_->SetNextScene("GAME");
	}
	//�^�C�g����escape�ŏI��
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
	//�Z���N�g
	StageSelect::GetInstance().DrawSprite();

	//�J�[�\��ui
	CursorUI::GetInstance().Draw();
}

void SceneStageSelect::DrawImgui()
{
}

