#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "Vec3.h"
#include "WorldMat.h"
#include "Camera.h"
#include <functional>
#include "CollisionTypes.h"


struct LevelData
{
	//�R���C�_�[�f�[�^
	struct ColliderData
	{
		//�R���C�_�[�g����
		bool isVailid = false;
		//���S���W
		Vec3 center = { 0,0,0 };
		//�X�P�[��
		Vec3 size = { 1,1,1 };
		//�R���C�_�[�^�C�v
		CollisionShapeType colliderType = SHAPE_UNKNOWN;
		//�n�ʂ�A�ǂƂ��Ĉ�����
		uint16_t attribute = 0;
	};

	//�I�u�W�F�N�g����̃f�[�^
	struct ObjectData
	{
		//worldMat
		std::unique_ptr<WorldMat> worldMat;
		//�t�@�C����
		std::string fileName;
		//�R���C�_�[�̃f�[�^
		ColliderData colliderData;
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