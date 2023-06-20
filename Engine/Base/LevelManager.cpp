#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"

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

	//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objData : JsonLevelLoader::Getinstance().levelData_->objects)
	{
		//�t�@�C��������o�^�ς݃��f��������
		Model* model = ModelManager::GetInstance().LoadModel(objData->fileName);
		//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		std::unique_ptr <Object> newObj = std::make_unique<Object>();
		//worldmat
		newObj->SetWorldMat_(*objData->worldMat.get());

		//�Z�b�g�œo�^
		objAndModels_.insert(std::make_pair(std::move(newObj), model));
	}

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
}

void LevelManager::Update()
{
	//.Obj���X�V
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		Model* model = it->second;

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
