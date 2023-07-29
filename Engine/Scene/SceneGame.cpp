#include "SceneGame.h"
#include "SceneManager.h"
#include "StageManager.h"


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
	if (LevelManager::GetInstance().GetGameOver())
	{
		sceneM_->SetNextScene("TITLE");
	}
	else if (LevelManager::GetInstance().GetGameClear())
	{
		clearCoolTime_++;

		//�N���A���o�p
		ClearEffect::GetInstance().Update();

		//�A�ł��Ĕ�΂��Ȃ��悤��
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && clearCoolTime_ >= CLEAR_COOL_TIME_MAX_)
		{
			ParticleManager::GetInstance()->ClearParticles();
			sceneM_->SetNextScene("TITLE");
		}
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
