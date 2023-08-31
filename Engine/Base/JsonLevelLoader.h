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

	//�e�m�[�h�f�[�^
	struct ParentNodeData
	{
		std::string parentName;
		std::string nodeName;
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
		//�E�F�[�u�ԍ�
		int32_t waveNum = -1;
		//�e�m�[�h��
		ParentNodeData parentNodeData;
	};

	//�J�����̃f�[�^
	struct CameraData
	{
		//�J����
		std::unique_ptr<Camera> camera;
		//�t�@�C����
		std::string fileName;
	};

	//���C�g�̃f�[�^
	struct LightData
	{
		//�ʒu
		Vec3 trans;
		//����
		Vec3 dir;
		//�X�P�[��(�����ȂǂɎg��)
		Vec3 scale;
		//�t�@�C����
		std::string fileName;
	};

	//�I�u�W�F�N�g�̔z��
	std::vector<std::unique_ptr<ObjectData>> objects;
	//�J����
	std::vector<std::unique_ptr<CameraData>> cameras;
	//���C�g
	std::vector<std::unique_ptr<LightData>> lights;
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

private:
	//�p�x�𓾂�
	Vec3 GetRot(const nlohmann::json::iterator& object);
	//�p�x���擾���A������g���x�N�g���𓾂�
	Vec3 GetRotDir(const nlohmann::json::iterator& object);
	//�ʒu�𓾂�
	Vec3 GetTrans(const nlohmann::json::iterator& object);
	//�X�P�[���𓾂�
	Vec3 GetScale(const nlohmann::json::iterator& object);


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
	//���C�g�̃f�[�^�ǂݍ���
	void LoadLightData(const nlohmann::json::iterator& object);
};