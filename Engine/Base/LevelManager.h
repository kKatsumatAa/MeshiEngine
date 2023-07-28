#pragma once
#include"JsonLevelLoader.h"
#include"Object.h"
#include"Camera.h"
#include"Gun.h"

//json�œǂݍ��񂾃��x�����Ǘ�����
class LevelManager final
{
private://�ϐ�
	//Object�ƃ��f���|�C���^�̃Z�b�g
	std::map<std::unique_ptr<Object>, Model*> objAndModels_;

	//�n�`�I�u�W�F�N�g�Ƃ��Ďg����
	bool isLandShape_ = false;


private://�֐�
	LevelManager() { ; }
	~LevelManager();

public:
	//�R�s�[�R���X�g���N�^�֎~
	LevelManager(const LevelManager& other) = delete;
	LevelManager& operator=(const LevelManager& other) = delete;

	static LevelManager& GetInstance();

	void StaticInitialize();
	//JsonLevelLoader�œǂݍ��񂾓��e�����ۂɃG���W����œǂݍ���
	void LoadLevelData(std::string fileName);

	//�t�@�C�����ɂ���ăN���X����
	void LoadObj(LevelData::ObjectData& objData);

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
	void CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, Model* model);



public:
	bool GetGameOver();
	bool GetGameClear();
};