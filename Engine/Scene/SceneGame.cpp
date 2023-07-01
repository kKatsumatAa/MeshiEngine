#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"


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

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	//�Q�[���X�s�[�h
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();
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
