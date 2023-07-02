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

void LevelManager::LoadLevelData(std::string fileName)
{
	//json�ǂݍ���
	JsonLevelLoader::Getinstance().LoadJsonFile(fileName);

	//initialize�̓x,���������Ⴄ�Ƃ��������Ȃ�
	objAndModels_.clear();
	CameraManager::GetInstance().Initialize();

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

Gun* LevelManager::GetChildGun(const LevelData::ObjectData& objData)
{
	//�q���e��������
	if (objData.childData && objData.childData->fileName == "gun")
	{
		//�t�@�C��������o�^�ς݃��f��������
		Model* model = ModelManager::GetInstance().LoadModel(objData.childData->fileName);
		//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		std::unique_ptr <Gun> newObj = {};

		//�C���X�^���X
		newObj = Gun::Create(std::move(objData.childData->worldMat));

		Gun* ans = newObj.get();

		//���ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
		newObj->CulcFrontVec();
		newObj->SetFrontVecTmp(newObj->GetFrontVec());

		//�Z�b�g�œo�^
		objAndModels_.insert(std::make_pair(std::move(newObj), model));

		return ans;
	}

	return nullptr;
}

bool LevelManager::GetGameOver()
{
	Player* p = nullptr;

	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "player")
		{
			p = dynamic_cast<Player*>(obj);
			return !p->GetIsAlive();
		}
	}
	return false;
}

bool LevelManager::GetGameClear()
{
	int count = 0;

	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "enemy" && obj->GetIsAlive())
		{
			return false;
		}
	}
	return true;
}

void LevelManager::LoadCharacter(LevelData::ObjectData& objData)
{
	//����Őe��������X�L�b�v(�o�^�ς݂Ȃ�worldMat��move()����ĂȂ��̂�)
	if (objData.fileName == "gun" && !objData.worldMat)
	{
		return;
	}


	//�t�@�C��������o�^�ς݃��f��������
	Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
	std::unique_ptr <Object> newObj = {};


	//�v���C���[�̏ꍇ
	if (objData.fileName == "player")
	{
		//player��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Player::Create(std::move(objData.worldMat), GetChildGun(objData));
	}
	//�G�̏ꍇ
	else if (objData.fileName == "enemy")
	{
		//enemy��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Enemy::Create(std::move(objData.worldMat), GetChildGun(objData));
		newObj->SetObjName("enemy");
	}
	//�e�̏ꍇ(�e������ꍇ�͊��ɓo�^���Ă���̂Ŕ�΂����)
	else if (objData.fileName == "gun")
	{
		newObj = std::make_unique<Object>();
		newObj->SetObjName("gun");
		newObj->SetWorldMat(std::move(objData.worldMat));
	}
	//���ɓ��Ă͂܂�Ȃ��Ƃ�
	else
	{
		newObj = std::make_unique<Object>();
		newObj->SetWorldMat(std::move(objData.worldMat));
	}

	//���ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//�Z�b�g�œo�^
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
	//.Obj���X�V
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
	}

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
}

void LevelManager::Draw(Camera* camera)
{
	//.Obj��\��
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		Model* model = it->second;

		obj->DrawModel(model, camera);
		obj->Draw();
	}
}
