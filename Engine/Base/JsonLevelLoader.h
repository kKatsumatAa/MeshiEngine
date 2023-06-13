#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "Vec3.h"
#include "WorldMat.h"

struct LevelData
{
	//�I�u�W�F�N�g����̃f�[�^
	struct ObjectData
	{
		//worldMat
		std::unique_ptr<WorldMat> worldMat;
		//�t�@�C����
		std::string fileName;
	};

	//�I�u�W�F�N�g�̔z��
	std::vector<std::unique_ptr<ObjectData>> objects;
};

class JsonLevelLoader final
{
private:
	std::string kDefaultBaseDirectory = "Resources/Level/";
	std::string kExtension = ".json";

public:
	//���x���f�[�^
	std::unique_ptr<LevelData> levelData;

private:
	JsonLevelLoader() { ; }
	~JsonLevelLoader() { ; }

	//�R�s�[�R���X�g���N�^�֎~
	JsonLevelLoader& operator=(const JsonLevelLoader& inst) = delete;
	JsonLevelLoader(const JsonLevelLoader& inst) = delete;

public:
	static JsonLevelLoader& Getinstance();
	void Initialize();

	//json�t�@�C���ǂݍ���
	void LoadJsonFile(std::string fileName);

	//�ċA�I�Ɏq����ǂݍ���
	void LoadRecursiveChildrenData(nlohmann::json::iterator object, WorldMat* worldMat = nullptr);
};