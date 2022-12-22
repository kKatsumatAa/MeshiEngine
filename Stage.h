#pragma once
#include"Object.h"

enum BLOCK
{
	NONE,
	NORMAL,//壊れるブロック
	HARD,//壊れない
};



class Stage
{
private:
	static const int mapLengthX = 11;
	static const int mapLengthY = 500;

	const float blockRadius = 3.0f;
	Object* blocks[mapLengthY][mapLengthX] = { nullptr };
	Model* model;

	const int mapLeftLength = mapLengthX / 2 * blockRadius * 2;

	//一列の最大ブロック数等
	static const int blockLineNumMax = mapLengthX - 4;
	static const int blockHardNumMax = mapLengthX - 6;
	int blockLineNum = blockLineNumMax;
	int blockHardNum = blockHardNumMax;

	int blockMapChip[mapLengthY][mapLengthX] = { 0 };


private:
	bool CollisionMapInternal(float left, float right, float down, float up, bool isBlockBreak = false);

public:
	void Initialize(Model* model);
	void GenerateStage();
	void GenerateBlock(int X, int Y);

	void BreakBlock(const int X, const int Y);

	//移動もこの関数内でやってもいいかも
	void CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround, bool isBlockBreak = false);

	void Update(Vec2& pos, Vec2& velocity, float radius);
	void Draw(ViewMat& view, ProjectionMat& projection);
};
