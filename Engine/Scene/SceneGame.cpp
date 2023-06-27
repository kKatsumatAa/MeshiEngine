#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//json
	JsonLevelLoader::Getinstance().Initialize();
	JsonLevelLoader::Getinstance().LoadJsonFile("level");
	//���x���}�l�[�W���[
	LevelManager::GetInstance().StaticInitialize();
	LevelManager::GetInstance().LoadLevelData();

	//�J�������Z�b�g
	//CameraManager::GetInstance().SetUsingCamera("playerCamera");

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Initialize();
}

void SceneGame::Update(PostPera* postPera)
{
	//�ۑ�p
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		if (postPera[0].effectFlags_.isMultiTex)
		{
			postPera[0].effectFlags_.isMultiTex = false;
			postPera[0].effectFlags_.isGlassFilter = true;
		}
		else if (postPera[0].effectFlags_.isGlassFilter)
		{
			postPera[0].effectFlags_.isGlassFilter = false;
			postPera[0].effectFlags_.isBloom = true;
		}
		else if (postPera[0].effectFlags_.isBloom)
		{
			postPera[0].effectFlags_.isBloom = false;
			postPera[0].effectFlags_.isCrossFilter = true;
		}
		else if (postPera[0].effectFlags_.isCrossFilter)
		{
			postPera[0].effectFlags_.isCrossFilter = false;
			postPera[0].effectFlags_.isGaussian = true;
		}
		else if (postPera[0].effectFlags_.isGaussian)
		{
			postPera[0].effectFlags_.isGaussian = false;
		}
		else if (!postPera[0].effectFlags_.isMultiTex && !postPera[0].effectFlags_.isGaussian)
		{
			postPera[0].effectFlags_.isMultiTex = true;
		}
	}

	//���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g��
	LevelManager::GetInstance().Update();

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
