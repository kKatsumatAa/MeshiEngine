#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "TouchableObject.h"
#include "MeshCollider.h"
#include "LandShape.h"


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

void LevelManager::LoadLevelData(int32_t fileIndex)
{
	//json�ǂݍ���
	JsonLevelLoader::Getinstance().LoadJsonFile(FILE_NAME_ + std::to_string(fileIndex));

	//�Z�b�g�ŕۑ����Ă�̂��N���A
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
		LoadObj(*objData.get());
	}
}

Weapon* LevelManager::GetChildWeapon(LevelData::ObjectData& objData)
{
	if (objData.childData == nullptr || (objData.childData->fileName != "gun" &&
		objData.childData->fileName != "sword"))
	{
		return nullptr;
	}

	//�t�@�C��������o�^�ς݃��f��������
	IModel* model = ModelManager::GetInstance().LoadModel(objData.childData->fileName);

	//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
	std::unique_ptr <Weapon> newObj = {};

	//�q���e��������
	if (objData.childData && objData.childData->fileName == "gun")
	{
		//�C���X�^���X
		newObj = Gun::Create(std::move(objData.childData->worldMat));
	}
	//�Ⴄ����
	else
	{

	}

	Weapon* ans = newObj.get();

	//���O
	newObj->SetObjName(objData.childData->fileName);

	//����n
	SetCollider(newObj.get(), objData, true);

	//���ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//���f���Z�b�g
	newObj->SetModel(model);

	//�Z�b�g�œo�^
	objAndModels_.insert(std::make_pair(std::move(newObj), model));

	if (ans != nullptr)
	{
		return ans;
	}

	return nullptr;
}

void LevelManager::CheckLandShapeObject(const LevelData::ObjectData& objData, bool& isLandShape)
{
	//�n�`�I�u�W�F�N�g
	if (objData.colliderData.attribute & COLLISION_ATTR_LANDSHAPE)
	{
		isLandShape = true;
	}
	else
	{
		isLandShape = false;
	}
}

void LevelManager::SetCollider(Object* obj, const LevelData::ObjectData& objData, bool isSettingCollider)
{
	//�^�C�v���Ȃ���΃R���C�_�[�����Ȃ̂�
	if (objData.colliderData.colliderType != CollisionShapeType::SHAPE_UNKNOWN)
	{
		CollisionShapeType type = objData.colliderData.colliderType;
		uint16_t attribute = objData.colliderData.attribute;

		if (isSettingCollider)
		{
			//���R���C�_�[
			if (type == COLLISIONSHAPE_SPHERE)
			{
				obj->SetCollider(std::make_unique<SphereCollider>());
			}
			//���b�V���R���C�_�[
			else if (type == COLLISIONSHAPE_MESH)
			{
				obj->SetCollider(std::make_unique<MeshCollider>());
			}
		}
		//���葮���Z�b�g
		obj->GetCollider()->SetAttribute(attribute);
	}
}

void LevelManager::CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, IModel* model)
{
	//�n�`�I�u�W�F�N�g�Ƃ��Ďg���̂Ȃ�
	if (isLandShape_)
	{
		obj = TouchableObject::Create(std::move(objData.worldMat), model);
	}
	//����ȊO�͕��ʂ�Object
	else
	{
		obj = std::make_unique<Object>();
		obj->SetWorldMat(std::move(objData.worldMat));
	}
}

bool LevelManager::GetGameOver()
{
	Player* p = nullptr;

	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "player")
		{
			return false;
		}
	}
	return true;
}

bool LevelManager::GetGameClear()
{
	int count = 0;

	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "enemy" && obj->GetIsAlive())
		{
			return false;
		}
	}
	return true;
}

void LevelManager::SetObjectIsDissolve(bool isDissolve, uint16_t attribute)
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetCollider()->GetAttribute() == attribute)
		{
			obj->SetisDissolve(isDissolve);
		}
	}
}

void LevelManager::SetIsDissolveT(float dissolveT, uint16_t attribute)
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetCollider() && obj->GetCollider()->GetAttribute() == attribute)
		{
			obj->SetDissolveT(dissolveT);
		}
	}
}

//-------------------------------------------------------------------------------------------
void LevelManager::LoadObj(LevelData::ObjectData& objData)
{
	//�e��������X�L�b�v(�o�^�ς݂Ȃ�worldMat��move()����Ė����̂�)
	if (/*objData.fileName == "gun" &&*/ !objData.worldMat)
	{
		return;
	}

	//�t�@�C��������o�^�ς݃��f��������
	IModel* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
	std::unique_ptr <Object> newObj = {};

	//�n�`�I�u�W�F�N�g�Ƃ��Ďg����
	CheckLandShapeObject(objData, isLandShape_);

	//�v���C���[�̏ꍇ
	if (objData.fileName == "player")
	{
		//player��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Player::Create(std::move(objData.worldMat), GetChildWeapon(objData));
	}
	//�G�̏ꍇ
	else if (objData.fileName == "enemy")
	{
		//enemy��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Enemy::Create(std::move(objData.worldMat), GetChildWeapon(objData));
	}
	//�e�̏ꍇ(�e������ꍇ�͊��ɓo�^���Ă���̂Œʂ�Ȃ�)
	else if (objData.fileName == "gun")
	{
		newObj = Gun::Create(std::move(objData.worldMat));
	}
	//�n�`�I�u�W�F�N�g�Ȃ�
	else if (isLandShape_)
	{
		newObj = LandShape::Create(std::move(objData.worldMat), model);
	}
	//�����̃I�u�W�F�N�g(��)
	else
	{
		newObj = std::make_unique<Object>();
	}

	//���O
	newObj->SetObjName(objData.fileName);

	//����n
	SetCollider(newObj.get(), objData, !isLandShape_);

	//���Ő��ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
	if (objData.fileName == "player")
	{
		newObj->CulcFrontVec();
		newObj->SetFrontVecTmp(newObj->GetFrontVec());
	}

	//���f���Z�b�g
	newObj->SetModel(model);

	newObj->PlayAnimation(true);

	//�Z�b�g�œo�^
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
	//.Obj���X�V
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->SetIsSilhouette(false);
	}

	//.Obj���X�V
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
	}

	//�R���C�_�[�Ő����Ă�t���O�I�t�ɂȂ��������
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
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
	//�`��
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Draw();
	}
}

void LevelManager::DrawImGui()
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->DrawImGui();
	}
}
