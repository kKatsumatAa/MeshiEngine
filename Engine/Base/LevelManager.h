#pragma once
#include"JsonLevelLoader.h"
#include"Object.h"
#include"Camera.h"
#include"Gun.h"
#include"ObjectManager.h"


//json�œǂݍ��񂾃��x�����Ǘ�����
class LevelManager final
{
private://�ϐ�
	//�n�`�I�u�W�F�N�g�Ƃ��Ďg����
	bool isLandShape_ = false;

	const std::string FILE_NAME_ = "level";

private:
	LightManager* lightManager_ = nullptr;
	float ambientColor_[3] = { 1,1,1 };
	float diffuseColor_[3] = { 1,1,1 };
	float specularColor_[3] = { 1,1,1 };
	float DirlightDir_[3] = { 0,-1.0f,1.0f };

	float pointPos_[3] = { 0,27.0f,-100.0f };


public:
	//�I�u�W�F�N�g�̃O���[�v��
	static const std::string S_OBJ_GROUP_NAME_;


private://�֐�
	LevelManager() { ; }
	~LevelManager();

public:
	//�R�s�[�R���X�g���N�^�֎~
	LevelManager(const LevelManager& other) = delete;
	LevelManager& operator=(const LevelManager& other) = delete;

	static LevelManager& GetInstance();

	void StaticInitialize();
	
	/// <summary>
	///JsonLevelLoader�œǂݍ��񂾓��e�����ۂɃG���W����œǂݍ���
	/// </summary>
	/// <param name="fileIndex">�t�@�C���̌�ɂ�����</param>
	void LoadLevelData(int32_t fileIndex);

	//�t�@�C�����ɂ���ăN���X����
	void LoadObj(LevelData::ObjectData& objData);
	//���C�g�𐶐��ݒ�
	void LoadLight(LevelData::LightData& lightData);

	//�ǂݍ��񂾂��̂𓮂���
	void Update();
	//�ǂݍ��񂾂��̂�`��
	void Draw(Camera* camera = nullptr);
	void DrawImGui();

private://�Q�[���I���W�i��
	//����̃|�C���^�Ԃ�
	Weapon* GetChildWeapon(LevelData::ObjectData& objData);

	//�n�`�I�u�W�F�N�g�Ƃ��Ďg�����`�F�b�N
	void CheckLandShapeObject(const LevelData::ObjectData& objData, bool& isLandShape);
	//�R���C�_�[�n�̃Z�b�g
	void SetCollider(Object* obj, const LevelData::ObjectData& objData, bool isSettingCollider);
	//�����ɂ����Object��TouchObject�����
	void CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, IModel* model);

public:
	void SetLightManager(LightManager* lightManager) { lightManager_ = lightManager; }
};