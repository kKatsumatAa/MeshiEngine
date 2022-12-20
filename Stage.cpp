#include "Stage.h"
#include <random>

//乱数シード生成器
static std::random_device seed_gen;
//メルセンヌツイスター
static std::mt19937_64 engine(seed_gen());
//乱数範囲
static std::uniform_int_distribution<int> mapRand(NONE, HARD);


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
		if (j % 4 == 0)
		{
			//ブロック最大数をリセット
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}

		for (int i = 0; i < mapLengthX; i++)
		{
			int blockNum = mapRand(engine);


			//何も置かない
			if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = 0;
			}
			//壊れるブロック
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0)
			{
				this->blockMapChip[j][i] = NORMAL;
				blockLineNum--;
				//登録
				blocks[j][i] = new Object;
				blocks[j][i]->worldMat->trans = { -mapLeftLength + (float)i * blockRadius * 2.0f, (float)-j * blockRadius * 2.0f,0 };
				blocks[j][i]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
				blocks[j][i]->worldMat->SetWorld();
			}
			//壊れない〃
			else if (blockNum == BLOCK::HARD && blockHardNum > 0 && blockLineNum > 0)
			{
				this->blockMapChip[j][i] = HARD;
				blockHardNum--;
				blockLineNum--;
				//登録
				blocks[j][i] = new Object;
				blocks[j][i]->worldMat->trans = { -mapLeftLength + (float)i * blockRadius * 2.0f, (float)-j * blockRadius * 2.0f,0 };
				blocks[j][i]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
				blocks[j][i]->worldMat->SetWorld();
			}
		}
	}

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

	Y = (int)(up / -(blockRadius * 2.0f));
	X = (int)((left + mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//左上

	Y = (int)(down / -(blockRadius * 2.0f));
	X = (int)((left + mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//左下

	Y = (int)(up / -(blockRadius * 2.0f));
	X = (int)((right + mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//右上

	Y = (int)(down / -(blockRadius * 2.0f));
	X = (int)((right + mapLeftLength) / (blockRadius * 2.0f));
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
	Vec3 pos_ = { pos.x + radius,pos.y - radius,pos.z };

	//マップチップとの判定
	//左右
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//移動でplayer位置が変わっている場合があるので角の更新

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up, isBlockBreak))//仮に進んだとしてそこにブロックがあるか
	{
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.1f, right + sign(velocity.x) * 0.1f, down, up, isBlockBreak))
		{
			pos_.x += sign(velocity.x) * 0.1f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//移動したので角の更新
		}

		velocity.x = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}

	pos.x += velocity.x;

	//y
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y, isBlockBreak))//仮に進んだとしてそこにブロックがあるか
	{
		if (velocity.y < 0)
		{
			isGround = true;
		}

		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.1f, up + sign(velocity.y) * 0.1f, isBlockBreak))
		{
			pos_.y += sign(velocity.y) * 0.1f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//移動したので角の更新
		}

		velocity.y = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}
	else
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
			if (blocks[j][i] != nullptr && (view.eye.y - blocks[j][i]->worldMat->trans.y) < mapLeftLength)
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



