#include "JsonLevelLoader.h"



JsonLevelLoader& JsonLevelLoader::Getinstance()
{
	static JsonLevelLoader inst;
	return inst;
}

void JsonLevelLoader::Initialize()
{
	levelData = std::make_unique<LevelData>();
}

void JsonLevelLoader::LoadJsonFile(const std::string& fileName)
{
	//�A�����ăt���p�X�𓾂�
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	//�t�@�C���X�g���[��
	std::ifstream file;

	//�t�@�C�����J��
	file.open(fullpath);
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

	//"name"�𕶎���Ƃ��Ď擾
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
	}
}

void JsonLevelLoader::LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent)
{
	//�v�f�ǉ�
	levelData->objects.emplace_back(std::make_unique<LevelData::ObjectData>());
	//���ǉ������v�f�̎Q�Ƃ𓾂�
	std::unique_ptr<LevelData::ObjectData>& objectData = levelData->objects.back();

	if (object->contains("file_name"))
	{
		//�t�@�C����
		objectData->fileName = (*object)["file_name"];
	}

	//�g�����X�t�H�[���̃p�����[�^�ǂݍ���
	nlohmann::json transform = (*object)["transform"];

	//�R���C�_�[�̃p�����[�^�ǂݍ���
	objectData->worldMat = std::make_unique<WorldMat>();
	// �e(��Ԗڂ̃I�u�W�F�N�g��nullptr������悤��)
	objectData->worldMat->parent = parent;
	//���s�ړ�
	objectData->worldMat->trans.x = (float)transform["translation"][1];
	objectData->worldMat->trans.y = (float)transform["translation"][2];
	objectData->worldMat->trans.z = -(float)transform["translation"][0];
	//�p�x�i���܂������ĂȂ������j
	objectData->worldMat->rot.x = -(float)transform["rotation"][1];
	objectData->worldMat->rot.y = -(float)transform["rotation"][2];
	objectData->worldMat->rot.z = (float)transform["rotation"][0];
	//�X�P�[��
	objectData->worldMat->scale.x = (float)transform["scaling"][1];
	objectData->worldMat->scale.y = (float)transform["scaling"][2];
	objectData->worldMat->scale.z = (float)transform["scaling"][0];

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
