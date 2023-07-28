#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"
#include "ClearEffectState.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//json
	JsonLevelLoader::Getinstance().Initialize();
	//���x���}�l�[�W���[
	LevelManager::GetInstance().StaticInitialize();
	LevelManager::GetInstance().LoadLevelData("level");

	//�J�������Z�b�g
	CameraManager::GetInstance().SetUsingCamera("playerCamera");

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Initialize();

	//�e
	BulletManager::GetInstance().Initialize();

	//
	ClearEffect::GetInstance().Initialize();

	ParticleManager::GetInstance()->SetBlendNum(ParticleManager::TRIANGLE);

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isVignette = true;
}

void SceneGame::Update()
{

#ifdef _DEBUG
	//���Z�b�g
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		LevelManager::GetInstance().LoadLevelData("level");
		ParticleManager::GetInstance()->ClearParticles();
		//�e
		BulletManager::GetInstance().Initialize();
		//�Q�[���X�s�[�h
		GameVelocityManager::GetInstance().Initialize();
	}
#endif

	//���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g��
	LevelManager::GetInstance().Update();

	//�e
	BulletManager::GetInstance().Update();

	//����
	CollisionManager::GetInstance()->CheckAllCollisions();

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Update();

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
	//�e
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	ParticleManager::GetInstance()->Draw();

	if (LevelManager::GetInstance().GetGameClear())
	{
		ClearEffect::GetInstance().Draw();
	}
}

void SceneGame::DrawSprite()
{
}

void SceneGame::DrawImgui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
}
