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
	static const int mapLengthX = 10;
	static const int mapLengthY = 500;

	const float blockRadius = 3.0f;
	std::vector<Object> blocks;
	Model* model;

	const int mapLeftLength = mapLengthX / 2 * blockRadius * 2;

	//一列の最大ブロック数等
	static const int blockLineNumMax = mapLengthX - 4;
	static const int blockHardNumMax = mapLengthY - 20;
	int blockLineNum = blockLineNumMax;
	int blockHardNum = blockHardNumMax;

	int blockMapChip[mapLengthY][mapLengthX] = { 0 };


private:
	bool CollisionMapInternal(int left, int right, int down, int up);

public:
	void Initialize(Model* model);
	void GenerateStage();

	//移動もこの関数内でやってもいいかも
	void CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround);

	void Update(Vec2& pos, Vec2& velocity, float radius);
	void Draw(ViewMat& view, ProjectionMat& projection);
};
