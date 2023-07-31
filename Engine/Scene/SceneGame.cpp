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
	//�X�e�[�W�ǂݍ���
	StageManager::GetInstance().LoadStage(1);

	//
	ClearEffect::GetInstance().Initialize();

	ParticleManager::GetInstance()->SetBlendNum(ParticleManager::TRIANGLE);

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isVignette = true;
}

void SceneGame::Update()
{

	//#ifdef _DEBUG
		//���Z�b�g
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		sceneM_->SetNextScene("GAME");
	}
	//#endif

		//�X�e�[�W�A�b�v�f�[�g
	StageManager::GetInstance().Update();

	//�V�[���J��
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
