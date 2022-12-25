#pragma once
#include"Object.h"
#include"EnemyManager.h"
#include"ItemManager.h"
#include"Room.h"

enum BLOCK
{
	NONE,
	NORMAL,//����u���b�N
	HARD,//���Ȃ�
	ROOML,//���̕���
	ROOMR//�E�̕���
};



class Stage
{
public:
	static const int mapNumX = 24;
	static const int mapNumY = 500;
	static const int stageBeginNumY = 10;
	//�X�e�[�W�̉���
	static const int mapDownMaxNum = mapNumY - mapNumY * 0.1f;

	bool isPlayerRoom = false;

private:
	//�O���N���X
	EnemyManager* enemyM;
	ItemManager* itemM;

	//�����n
	std::vector<Room *> rooms;
	std::vector<int> roomNumY;

	//���ǂ̕�
	const int hardWallNum = mapNumX / 3;
	//�u���b�N�̔��a
	const float blockRadius = 3.0f;

	//Object�̃C���X�^���X�̃|�C���^������
	Object* blocks[mapNumY][mapNumX] = { nullptr };
	Model* model;

	//3D��(0,0)�����S�Ȃ̂ō��ɂ��炷���߂̕�
	const float mapLeftLength = ((float)mapNumX * (float)blockRadius * 2.0f) / 2.0f;

	//���̍ő�u���b�N����
	static const int blockLineNumMax = mapNumX - 16;
	static const int blockHardNumMax = mapNumX - 16;
	int blockLineNum = blockLineNumMax;
	int blockHardNum = blockHardNumMax;

	int blockMapChip[mapNumY][mapNumX] = { 0 };

	//���������ۂɎg��
	static const int beginRoomY = mapDownMaxNum + 20;
	static const int beginRoomYLeft = mapDownMaxNum + 20 + 15;
	static const int roomLengthX = 10;
	static const int roomLengthY = 7;

	//�G��
	static const int enemyLineMax = 3;
	int enemyLineNum = enemyLineMax;

	Vec3 beforeRoomPos;

private:
	int CollisionMapInternal(float left, float right, float down, float up, bool isBlockBreak = false);

public:
	~Stage();

	void Initialize(Model* model, EnemyManager* enemyM, ItemManager* itemM);
	void GenerateStage();
	void GenerateBlock(int X, int Y);
	void GenerateHardBlock(int X, int Y);
	void GenerateRoom();
	void GenerateRoomInternal();

	Vec3 MapChipTransVec3(int X, int Y);

	void DeleteBlock(int X, int Y);

	void BreakBlock(const int X, const int Y);

	//�ړ������̊֐����ł���Ă���������
	void CollisionMap(Collider* collider, bool& isGround, bool isBlockBreak = false);

	void CollisionRoom(Collider* collider);

	void Update(Vec2& pos, Vec2& velocity, float radius);
	void Draw(ViewMat& view, ProjectionMat& projection);
};
