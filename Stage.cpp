#include "Stage.h"
#include <random>

//乱数シード生成器
static std::random_device seed_gen;
//メルセンヌツイスター
static std::mt19937_64 engine(seed_gen());
//乱数範囲
static std::uniform_int_distribution<int> mapRand(NONE, HARD + 3);
static std::uniform_int_distribution<int> hardRand(NONE, HARD + 2);


void Stage::Initialize(Model* model)
{
	for (int j = 0; j < mapLengthY; j++)
	{
		for (int i = 0; i < mapLengthX; i++)
		{
			delete blocks[j][i];
			blocks[j][i] = nullptr;
		}
	}
	this->model = model;
}

void Stage::GenerateStage()
{
	for (int j = 0; j < mapLengthY; j++)
	{
		if (j % 6 == 0)
		{
			//ブロック最大数をリセット
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}

		int hard[2] = { hardRand(engine),hardRand(engine) };

		for (int i = 0; i < mapLengthX; i++)
		{
			int blockNum = mapRand(engine);

			//左右の端は壊れないブロックを置く
			if (i == 0 || i == mapLengthX - 1)
			{
				this->blockMapChip[j][i] = HARD;
				GenerateBlock(i, j);
			}
			//壊れない〃
			else if (((i == 1 && hard[0] == HARD) || (hard[1] == HARD && i == mapLengthX - 2))
				&& blockHardNum > 0 && j > 10)
			{
				this->blockMapChip[j][i] = HARD;
				blockHardNum--;
				blockLineNum--;

				GenerateBlock(i, j);
			}
			//何も置かない
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = 0;
			}
			//壊れるブロック
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0 && j > 10)
			{
				this->blockMapChip[j][i] = NORMAL;
				blockLineNum--;

				GenerateBlock(i, j);
			}
		}
	}

}

void Stage::GenerateBlock(int X, int Y)
{
	//登録
	blocks[Y][X] = new Object;                                                     //中心座標なので左上座標に合わせる（blockRadiusを足したり）
	blocks[Y][X]->worldMat->trans = { -mapLeftLength + (float)X * blockRadius * 2.0f + blockRadius, (float)-Y * blockRadius * 2.0f - blockRadius,0 };
	blocks[Y][X]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
	blocks[Y][X]->worldMat->SetWorld();
}

void Stage::BreakBlock(const int X, const int Y)
{
	if (blockMapChip[Y][X] == NORMAL) {
		blockMapChip[Y][X] = NONE;
		delete blocks[Y][X];
		blocks[Y][X] = nullptr;
	}
	//演出など

}

bool Stage::CollisionMapInternal(float left, float right, float down, float up, bool isBlockBreak)
{
	int X, Y;

	Y = (up / -(blockRadius * 2.0f));
	X = ((left + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//左上

	Y = (down / -(blockRadius * 2.0f));
	X = ((left + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//左下

	Y = (up / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//右上

	Y = (down / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//右下

	return false;
}

void Stage::CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround, bool isBlockBreak)
{
	//何故か判定おかしいので少し変えてあげる
	Vec3 pos_ = { pos.x  ,pos.y  ,pos.z };

	//マップチップとの判定
	//左右
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//移動でplayer位置が変わっている場合があるので角の更新

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//仮に進んだとしてそこにブロックがあるか
	{
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.01f, right + sign(velocity.x) * 0.01f, down, up, isBlockBreak))
		{
			pos_.x += sign(velocity.x) * 0.01f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//移動したので角の更新
		}

		velocity.x = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}

	pos.x += velocity.x;

	//y
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y))//仮に進んだとしてそこにブロックがあるか
	{
		if (CollisionMapInternal(left, right, down - 1.0f, up))
		{
			isGround = true;
		}

		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.01f, up + sign(velocity.y) * 0.01f, isBlockBreak))
		{
			pos_.y += sign(velocity.y) * 0.01f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//移動したので角の更新
		}

		velocity.y = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}
	else if (!CollisionMapInternal(left, right, down - 1.0f, up))
	{
		isGround = false;
	}

	pos.y += velocity.y;
}

void Stage::Update(Vec2& pos, Vec2& velocity, float radius)
{
}

void Stage::Draw(ViewMat& view, ProjectionMat& projection)
{
	int count = 0;

	for (int j = 0; j < mapLengthY; j++)
	{
		for (int i = 0; i < mapLengthX; i++)
		{
			if (blocks[j][i] != nullptr && (view.eye.y - blocks[j][i]->worldMat->trans.y) < mapLeftLength + 20.0f)
			{
				if (blockMapChip[j][i] == NORMAL)
				{
					blocks[j][i]->DrawCube3D(blocks[j][i]->worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f });
					count++;
				}
				if (blockMapChip[j][i] == HARD)
				{
					blocks[j][i]->DrawCube3D(blocks[j][i]->worldMat, &view, &projection, { 1.0f,0,0,1.0f });
					count++;
				}
			}
		}
	}
}



