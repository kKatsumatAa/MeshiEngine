#include "StageManager.h"
#include "StageState.h"


StageManager& StageManager::GetInstance()
{
	static StageManager sInst;
	return sInst;
}

void StageManager::LoadStage(int32_t stageIndex)
{
	//�v���C���[ui
	PlayerUI::GetInstance().Initialize();

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

	//�G�}�l�[�W���[
	EnemyManager::GetInstance().Initialize();

	//�t���O
	isClear_ = false;
	isGameOver_ = false;

	//
	ChangeState("BEGINING");
}


void StageManager::ChangeState(const std::string& name)
{
	state_.reset();
	state_ = std::move(StageState::GetState(name));
	state_->Initialize();
}

void StageManager::Update()
{
	state_->Update();
}

void StageManager::Draw()
{
	state_->Draw();
}

void StageManager::DrawSprite()
{
	state_->DrawSprite();
}

void StageManager::DrawImGui()
{
	state_->DrawImgui();
}
