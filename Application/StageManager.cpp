#include "StageManager.h"


StageManager& StageManager::GetInstance()
{
	static StageManager sInst;
	return sInst;
}

void StageManager::LoadStage(int32_t stageIndex)
{
	//json
	JsonLevelLoader::Getinstance().Initialize();
	//���x���}�l�[�W���[
	LevelManager::GetInstance().StaticInitialize();
	LevelManager::GetInstance().LoadLevelData(stageIndex);

	//�`���[�g���A�����ǂݍ���
	Tutorial::GetInstance().LoadTutorialData(stageIndex);

	//������
	StageManager::Initialize();
}

void StageManager::Initialize()
{
	//�J�������Z�b�g
	CameraManager::GetInstance().SetUsingCamera("playerCamera");

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Initialize();

	//�e
	BulletManager::GetInstance().Initialize();
}


void StageManager::Update()
{
	//���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g��
	LevelManager::GetInstance().Update();

	//�e
	BulletManager::GetInstance().Update();

	//����
	CollisionManager::GetInstance()->CheckAllCollisions();

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Update();

	//�`���[�g���A��
	Tutorial::GetInstance().Update();
}

void StageManager::Draw()
{
	//�e
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	ParticleManager::GetInstance()->Draw();

	//�`���[�g���A��
	Tutorial::GetInstance().Draw();

	//�N���A���o
	if (LevelManager::GetInstance().GetGameClear())
	{
		ClearEffect::GetInstance().Draw();
	}
}

void StageManager::DrawSprite()
{
}

void StageManager::DrawImGui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
}
