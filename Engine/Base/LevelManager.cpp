#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"

LevelManager::~LevelManager()
{
	objAndModels_.clear();
}

LevelManager& LevelManager::GetInstance()
{
	static LevelManager sInst;
	return sInst;
}

void LevelManager::StaticInitialize()
{
}

void LevelManager::LoadLevelData()
{
	//initialize�̓x,���������Ⴄ�Ƃ��������Ȃ�
	objAndModels_.clear();

	//���x���f�[�^����J�������擾
	for (auto& cameraData : JsonLevelLoader::Getinstance().levelData_->cameras)
	{
		//�J����������J�������쐬
		CameraManager::GetInstance().AddCamera(cameraData->fileName);
		//camera�̃p�����[�^�Z�b�g
		Camera* camera = CameraManager::GetInstance().GetCamera(cameraData->fileName);
		camera->SetEye(cameraData->camera->GetEye());
		camera->SetTarget(cameraData->camera->GetTarget());
		camera->SetUp(cameraData->camera->GetUp());

		//�Ƃ肠�����J�������Z�b�g
		CameraManager::GetInstance().SetUsingCamera(cameraData->fileName);

		//cameraManager�ɓo�^����Ă�̂ł������ł͓o�^���Ȃ�
	}

	//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objData : JsonLevelLoader::Getinstance().levelData_->objects)
	{		
		//
		LoadCharacter(*objData.get());
	}
}

void LevelManager::LoadCharacter(const LevelData::ObjectData& objData)
{
	//�t�@�C��������o�^�ς݃��f��������
	Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
	std::unique_ptr <Object> newObj = {};

	//�v���C���[�̏ꍇ
	if (objData.fileName == "player")
	{
		//player��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Player::Create();
	}
	//�G�̏ꍇ
	else if (objData.fileName == "enemy")
	{
		//enemy��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Enemy::Create();
	}
	//���ɓ��Ă͂܂�Ȃ��Ƃ�
	else
	{
		newObj = std::make_unique<Object>();
	}



	//worldmat
	newObj->SetWorldMat_(*objData.worldMat.get());

	//���ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//�Z�b�g�œo�^
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
	//�R���C�_�[�Ő����Ă�t���O�I�t�ɂȂ��������
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		
		if (!obj->GetIsAlive())
		{
			objAndModels_.erase(it);
			it = objAndModels_.begin();
		}
	}

	//.Obj���X�V
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
	}
}

void LevelManager::Draw(Camera* camera)
{
	//.Obj��\��
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		Model* model = it->second;

		obj->DrawModel(model, camera);
	}
}
