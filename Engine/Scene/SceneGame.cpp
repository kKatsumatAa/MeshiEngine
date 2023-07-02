#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"


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
}

void SceneGame::Update(PostPera* postPera)
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

	ClearEffect::GetInstance().Update();

	//�V�[���J��
	if (LevelManager::GetInstance().GetGameOver())
	{
		sceneM_->ChangeScene("TITLE");
	}
	else if (LevelManager::GetInstance().GetGameClear())
	{
		clearCoolTime_++;

		ClearEffect::GetInstance().Update();

		//�A�ł��Ĕ�΂��Ȃ��悤��
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && clearCoolTime_ >= CLEAR_COOL_TIME_MAX_)
		{
			sceneM_->ChangeScene("TITLE");
		}
	}
}

void SceneGame::Draw()
{
	//�Q�[���X�s�[�h
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

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

void SceneGame::DrawPostEffect()
{
	//postPera.Draw();
}

void SceneGame::DrawPostEffect2()
{
	//postPera.Draw2All();
}
