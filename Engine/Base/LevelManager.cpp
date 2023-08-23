#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "TouchableObject.h"
#include "MeshCollider.h"
#include "LandShape.h"
#include "ObjectManager.h"


const std::string LevelManager::S_OBJ_GROUP_NAME_ = LevelManager::GetInstance().FILE_NAME_;


LevelManager::~LevelManager()
{
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

	//�I�u�W�F�N�g�A�J�������N���A
	ObjectManager::GetInstance().ClearAllObj();
	CameraManager::GetInstance().Initialize();
	lightManager_->InitializeCount();

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

	//���x���f�[�^���烉�C�g���擾
	for (auto& lightData : JsonLevelLoader::Getinstance().levelData_->lights)
	{
		//
		LoadLight(*lightData.get());
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
	SetCollider(newObj.get(), *objData.childData, true);

	//���ʃx�N�g��(obj�̊p�x�ɂ���ĉ�],��]��̃x�N�g������b���ʂƂ���)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//���f���Z�b�g
	newObj->SetModel(model);

	//�o�^
	ObjectManager::GetInstance().AddObject(S_OBJ_GROUP_NAME_, std::move(newObj));

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
	else if (objData.fileName.find("enemy") != std::string::npos)
	{
		//enemy��Object�N���X���p�����Ă�̂Ń|�����[�t�B�Y��
		newObj = Enemy::Create(std::move(objData.worldMat), objData.waveNum, GetChildWeapon(objData));
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


	//���f���Z�b�g
	newObj->SetModel(model);

	newObj->PlayAnimation(true);

	//obj�o�^
	ObjectManager::GetInstance().AddObject(S_OBJ_GROUP_NAME_, std::move(newObj));
}

void LevelManager::LoadLight(LevelData::LightData& lightData)
{
	//�������C�g�Ȃ�
	if (lightData.fileName.find("dir") != std::string::npos)
	{
		lightManager_->SetDirLightActive(lightManager_->GetDirLCount(), true);

		lightManager_->SetDirLightDir(lightManager_->GetDirLCount(),
			{ lightData.dir.x_,lightData.dir.y_,lightData.dir.z_ });

		lightManager_->AddDirLCount();
	}
	//�|�C���g���C�g�Ȃ�
	else if (lightData.fileName.find("point") != std::string::npos)
	{
		lightManager_->SetPointLightActive(lightManager_->GetPointLCount(), true);

		lightManager_->SetPointLightPos(lightManager_->GetPointLCount(),
			{ lightData.trans.x_,lightData.trans.y_ ,lightData.trans.z_ });

		lightManager_->SetPointLightAtten(lightManager_->GetPointLCount(),
			{ lightData.scale.z_,lightData.scale.x_,lightData.scale.y_ });

		lightManager_->AddPointLCount();
	}
	//�X�|�b�g���C�g�Ȃ�
	else if (lightData.fileName.find("spot") != std::string::npos)
	{
		lightManager_->SetSpotLightActive(lightManager_->GetSpotLCount(), true);

		lightManager_->SetSpotLightPos(lightManager_->GetSpotLCount(),
			{ lightData.trans.x_,lightData.trans.y_ ,lightData.trans.z_ });

		lightManager_->SetSpotLightAtten(lightManager_->GetSpotLCount(),
			{ lightData.scale.z_,lightData.scale.x_,lightData.scale.y_ });

		lightManager_->SetSpotLightDir(lightManager_->GetSpotLCount(),
			{ lightData.dir.x_,lightData.dir.y_,lightData.dir.z_ });

		lightManager_->AddSpotLCount();
	}
}

void LevelManager::Update()
{

}

void LevelManager::Draw(Camera* camera)
{
}

void LevelManager::DrawImGui()
{

}
