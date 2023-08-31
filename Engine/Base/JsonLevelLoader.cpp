#include "JsonLevelLoader.h"
#include "Util.h"
#include "CollisionManager.h"
#include <fstream>
#include <sstream>

const std::string JsonLevelLoader::S_DEFAULT_BASE_DIRECTORY_ = "Resources/Level/levelData/";
const std::string JsonLevelLoader::S_EXTENSION_ = ".json";


JsonLevelLoader& JsonLevelLoader::Getinstance()
{
	static JsonLevelLoader sInst;
	return sInst;
}

void JsonLevelLoader::Initialize()
{
	levelData_.reset();
	levelData_ = std::make_unique<LevelData>();
}

void JsonLevelLoader::LoadJsonFile(const std::string& fileName)
{
	Initialize();

	//�A�����ăt���p�X�𓾂�
	const std::string FULL_PATH = S_DEFAULT_BASE_DIRECTORY_ + fileName + S_EXTENSION_;

	//�t�@�C���X�g���[��
	std::ifstream file;

	//�t�@�C�����J��
	file.open(FULL_PATH);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	//JSON�����񂩂�𓀂����f�[�^
	nlohmann::json deserialized;

	//��
	file >> deserialized;

	//���������x���f�[�^
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name_"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();
	//���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("scene") == 0);

	//���x���f�[�^�i�[�p�C���X�^���X�𐶐�
	//level

	//"objects"�̑S�I�u�W�F�N�g�𑖍�
	for (nlohmann::json::iterator object = deserialized["objects"].begin(); object != deserialized["objects"].end(); object++)
	{
		assert(object->contains("type"));

		//��ʂ��擾
		std::string type = (*object)["type"].get<std::string>();

		//��ނ��Ƃ̏���
		//Mesh
		if (type.compare("MESH") == 0)
		{
			//�ċA�I
			LoadRecursiveChildrenData(object);
		}

		//Camera
		if (type.compare("CAMERA") == 0)
		{
			LoadCameraData(object);
		}

		//���C�g
		if (type.compare("LIGHT") == 0)
		{
			LoadLightData(object);
		}
	}
}

void JsonLevelLoader::LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent)
{
	//�v�f�ǉ�
	levelData_->objects.emplace_back(std::make_unique<LevelData::ObjectData>());
	//���ǉ������v�f�̎Q�Ƃ𓾂�
	std::unique_ptr<LevelData::ObjectData>& objectData = levelData_->objects.back();

	if (object->contains("file_name"))
	{
		//�t�@�C����
		objectData->fileName = (*object)["file_name"];
	}

	//�R���C�_�[�̃p�����[�^�ǂݍ���
	objectData->worldMat = std::make_unique<WorldMat>();
	// �e(��Ԗڂ̃I�u�W�F�N�g��nullptr������悤��)
	objectData->worldMat->parent_ = parent;
	//���s�ړ�
	objectData->worldMat->trans_ = GetTrans(object);
	//�p�x�i���܂������ĂȂ������j
	objectData->worldMat->rot_ = GetRot(object);
	//�X�P�[��
	objectData->worldMat->scale_ = GetScale(object);


	//�E�F�[�u�ԍ�
	if (object->contains("wave_num"))
	{
		std::string numStr = (*object)["wave_num"];
		objectData->waveNum = std::stoi(numStr);
	}

	//�e�m�[�h
	if (object->contains("parent_node"))
	{
		nlohmann::json parent_node = (*object)["parent_node"];
		objectData->parentNodeData.parentName = parent_node["parent_name"];
		objectData->parentNodeData.nodeName = parent_node["node_name"];
	}

	//�����蔻��f�[�^
	//�g�����X�t�H�[���̃p�����[�^�ǂݍ���
	nlohmann::json collider = (*object)["collider"];
	if (collider.size())
	{
		//�R���C�_�[���
		std::string typeStr = collider["type"];
		if (typeStr == "SPHERE")
		{
			objectData->colliderData.colliderType = CollisionShapeType::COLLISIONSHAPE_SPHERE;
		}
		else if (typeStr == "MESH")
		{
			objectData->colliderData.colliderType = CollisionShapeType::COLLISIONSHAPE_MESH;
		}
		//���葮��
		std::string line = (std::string)collider["attribute"];
		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);
		// ','��؂�ōs�̐擪��������擾
		std::string key;
		uint16_t attribute = 0;
		while (std::getline(line_stream, key, ','))
		{
			if (key == "ALLIES")
			{
				attribute |= COLLISION_ATTR_ALLIES;
			}
			else if (key == "ENEMYS")
			{
				attribute |= COLLISION_ATTR_ENEMYS;
			}
			else if (key == "LANDSHAPE")
			{
				attribute |= COLLISION_ATTR_LANDSHAPE;
			}
			else if (key == "ITEMS")
			{
				attribute |= COLLISION_ATTR_ITEMS;
			}
		}
		objectData->colliderData.attribute = attribute;

		//���S�ʒu
		objectData->colliderData.center.x_ = (float)collider["center"][1];
		objectData->colliderData.center.y_ = (float)collider["center"][2];
		objectData->colliderData.center.z_ = -(float)collider["center"][0];
		//�X�P�[��
		objectData->colliderData.size.x_ = (float)collider["size"][1];
		objectData->colliderData.size.y_ = (float)collider["size"][2];
		objectData->colliderData.size.z_ = (float)collider["size"][0];
	}

	//�e��������A����Ɏ����̃f�[�^�����
	if (parent)
	{
		auto itr = levelData_->objects.end();
		std::advance(itr, -2);//end�͉����Ȃ��A���̈�O�����̎����������Ă�̂œ��߂�
		itr->get()->childData = objectData.get();
	}

	//�q��������
	if ((*object).contains("children"))
	{
		//�q��for���ő���
		for (nlohmann::json::iterator child = (*object)["children"].begin(); child != (*object)["children"].end(); child++)
		{
			//�q���ċA�ő���
			LoadRecursiveChildrenData(child, objectData->worldMat.get());
		}
	}
}


//-----------------------------------------------------------------------
Vec3 JsonLevelLoader::GetRot(const nlohmann::json::iterator& object)
{
	//�g�����X�t�H�[���̃p�����[�^�ǂݍ���
	nlohmann::json transform = (*object)["transform"];

	Vec3 rot = {
			AngletoRadi(-(float)transform["rotation"][1]),
			AngletoRadi(-(float)transform["rotation"][2]),
			AngletoRadi((float)transform["rotation"][0])
	};
	return rot;
}

Vec3 JsonLevelLoader::GetRotDir(const nlohmann::json::iterator& object)
{
	//�p�x�𓾂�
	WorldMat targetWorldMat;
	targetWorldMat.rot_ = GetRot(object);

	targetWorldMat.CalcWorldMat();
	//���Ɍ������x�N�g�����^�[�Q�b�g�̌��Ƃ���
	Vec3 dir = { 0,0,1.0f };
	//�ǂݍ��񂾊p�x�Ń^�[�Q�b�g����]
	Vec3xM4(dir, targetWorldMat.matWorld_, 0);


	return dir;
}

Vec3 JsonLevelLoader::GetTrans(const nlohmann::json::iterator& object)
{
	//�g�����X�t�H�[���̃p�����[�^�ǂݍ���
	nlohmann::json transform = (*object)["transform"];

	return { (float)transform["translation"][1], (float)transform["translation"][2], -(float)transform["translation"][0] };
}

Vec3 JsonLevelLoader::GetScale(const nlohmann::json::iterator& object)
{
	//�g�����X�t�H�[���̃p�����[�^�ǂݍ���
	nlohmann::json transform = (*object)["transform"];

	return{ (float)transform["scaling"][1],	(float)transform["scaling"][2],(float)transform["scaling"][0] };
}

//-------------------------------------------------------------------------------
//�J�����̃f�[�^�ǂݍ���
void JsonLevelLoader::LoadCameraData(const nlohmann::json::iterator& object)
{
	//�v�f�ǉ�
	levelData_->cameras.emplace_back(std::make_unique<LevelData::CameraData>());
	//���ǉ������v�f�̎Q�Ƃ𓾂�
	std::unique_ptr<LevelData::CameraData>& objectData = levelData_->cameras.back();

	if (object->contains("file_name"))
	{
		//�t�@�C����
		objectData->fileName = (*object)["file_name"];
	}

	//�J�����쐬
	objectData->camera = std::make_unique<Camera>();
	objectData->camera->Initialize();
	//���s�ړ�
	objectData->camera->SetEye(GetTrans(object));
	//�^�[�Q�b�g
	objectData->camera->SetTarget(GetRotDir(object));
}

void JsonLevelLoader::LoadLightData(const nlohmann::json::iterator& object)
{
	//�v�f�ǉ�
	levelData_->lights.emplace_back(std::make_unique<LevelData::LightData>());
	//���ǉ������v�f�̎Q�Ƃ𓾂�
	std::unique_ptr<LevelData::LightData>& objectData = levelData_->lights.back();

	if (object->contains("file_name"))
	{
		//�t�@�C����
		objectData->fileName = (*object)["file_name"];
	}

	//���s�ړ�
	objectData->trans = GetTrans(object);
	//�^�[�Q�b�g
	objectData->dir = GetRotDir(object);
	//�X�P�[��(�����ȂǂɎg��)
	objectData->scale = GetScale(object);
}
