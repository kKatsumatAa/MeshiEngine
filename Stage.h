#pragma once
#include"Object.h"
#include"EnemyManager.h"
#include"ItemManager.h"
#include"Room.h"

enum BLOCK
{
	NONE,
	NORMAL,//壊れるブロック
	HARD,//壊れない
	ROOML,//左の部屋
	ROOMR//右の部屋
};



class Stage
{
public:
	static const int mapNumX = 24;
	static const int mapNumY = 500;
	static const int stageBeginNumY = 10;
	//ステージの下限
	static const int mapDownMaxNum = mapNumY - mapNumY * 0.1f;

	bool isPlayerRoom = false;

private:
	//外部クラス
	EnemyManager* enemyM;
	ItemManager* itemM;

	//部屋系
	std::vector<Room *> rooms;
	std::vector<int> roomNumY;

	//両壁の幅
	const int hardWallNum = mapNumX / 3;
	//ブロックの半径
	const float blockRadius = 3.0f;

	//Objectのインスタンスのポインタを入れる
	Object* blocks[mapNumY][mapNumX] = { nullptr };
	Model* model;

	//3Dで(0,0)が中心なので左にずらすための幅
	const float mapLeftLength = ((float)mapNumX * (float)blockRadius * 2.0f) / 2.0f;

	//一列の最大ブロック数等
	static const int blockLineNumMax = mapNumX - 16;
	static const int blockHardNumMax = mapNumX - 16;
	int blockLineNum = blockLineNumMax;
	int blockHardNum = blockHardNumMax;

	int blockMapChip[mapNumY][mapNumX] = { 0 };

	//部屋を作る際に使う
	static const int beginRoomY = mapDownMaxNum + 20;
	static const int beginRoomYLeft = mapDownMaxNum + 20 + 15;
	static const int roomLengthX = 10;
	static const int roomLengthY = 7;

	//敵数
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

	//移動もこの関数内でやってもいいかも
	void CollisionMap(Collider* collider, bool& isGround, bool isBlockBreak = false);

	void CollisionRoom(Collider* collider);

	void Update(Vec2& pos, Vec2& velocity, float radius);
	void Draw(ViewMat& view, ProjectionMat& projection);
};
