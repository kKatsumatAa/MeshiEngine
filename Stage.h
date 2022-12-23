#pragma once
#include"Object.h"
#include"EnemyManager.h"

enum BLOCK
{
	NONE,
	NORMAL,//壊れるブロック
	HARD,//壊れない
};



class Stage
{
public:
	static const int mapLengthX = 24;
	static const int mapLengthY = 500;
	static const int stageBeginY = 10;

	bool isPlayerRoom = false;

private:
	EnemyManager* enemyM;
	
	const int hardWallLength = mapLengthX / 3;
	const float blockRadius = 3.0f;
	Object* blocks[mapLengthY][mapLengthX] = { nullptr };
	Model* model;

	const float mapLeftLength = ((float)mapLengthX * (float)blockRadius * 2.0f) / 2.0f;

	//一列の最大ブロック数等
	static const int blockLineNumMax = mapLengthX - 16;
	static const int blockHardNumMax = mapLengthX - 16;
	int blockLineNum = blockLineNumMax;
	int blockHardNum = blockHardNumMax;

	int blockMapChip[mapLengthY][mapLengthX] = { 0 };

	//敵数
	static const int enemyLineMax = 3;
	int enemyLineNum = enemyLineMax;

private:
	bool CollisionMapInternal(float left, float right, float down, float up, bool isBlockBreak = false);

public:
	~Stage();

	void Initialize(Model* model, EnemyManager* enemyM);
	void GenerateStage();
	void GenerateBlock(int X, int Y);
	void GenerateHardBlock(int X, int Y);
	void GenerateRoom();

	Vec3 MapChipTransVec3(int X, int Y);

	void DeleteBlock(int X, int Y);

	void BreakBlock(const int X, const int Y);

	//移動もこの関数内でやってもいいかも
	void CollisionMap(Collider* collider, bool& isGround, bool isBlockBreak = false);

	void Update(Vec2& pos, Vec2& velocity, float radius);
	void Draw(ViewMat& view, ProjectionMat& projection);
};
