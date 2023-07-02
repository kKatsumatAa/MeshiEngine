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
	void LoadCharacter( LevelData::ObjectData& objData);

	//�ǂݍ��񂾂��̂𓮂���
	void Update();
	//�ǂݍ��񂾂��̂�`��
	void Draw(Camera* camera = nullptr);

private://�Q�[���I���W�i��
	Gun* GetChildGun(const LevelData::ObjectData& objData);
public:
	bool GetGameOver();
	bool GetGameClear();
};