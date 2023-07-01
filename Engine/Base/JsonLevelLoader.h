#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "Vec3.h"
#include "WorldMat.h"
#include "Camera.h"
#include <functional>


struct LevelData
{
	//�I�u�W�F�N�g����̃f�[�^
	struct ObjectData
	{
		//worldMat
		std::unique_ptr<WorldMat> worldMat;
		//�t�@�C����
		std::string fileName;
		//�q�̃f�[�^
		ObjectData* childData;
	};

	//�J�����̃f�[�^
	struct CameraData
	{
		//worldMat
		std::unique_ptr<Camera> camera;
		//�t�@�C����
		std::string fileName;
	};

	//�I�u�W�F�N�g�̔z��
	std::vector<std::unique_ptr<ObjectData>> objects;
	//�J����
	std::vector<std::unique_ptr<CameraData>> cameras;
};

class JsonLevelLoader final
{
private:
	static const std::string S_DEFAULT_BASE_DIRECTORY_;
	static const std::string S_EXTENSION_;

public:
	//���x���f�[�^
	std::unique_ptr<LevelData> levelData_ = nullptr;

private:
	JsonLevelLoader() { ; }
	~JsonLevelLoader() { ; }

public:
	//�R�s�[�R���X�g���N�^�֎~
	JsonLevelLoader& operator=(const JsonLevelLoader& inst) = delete;
	JsonLevelLoader(const JsonLevelLoader& inst) = delete;

	static JsonLevelLoader& Getinstance();
	void Initialize();

	//json�t�@�C���ǂݍ���
	void LoadJsonFile(const std::string& fileName);

	//�ċA�I�Ɏq����ǂݍ���(�I�u�W�F�N�g�f�[�^)
	void LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent = nullptr);
	//�J�����̃f�[�^�ǂݍ���
	void LoadCameraData(const nlohmann::json::iterator& object);
};