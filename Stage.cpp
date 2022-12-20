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
	blocks.clear();
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
				Object block;
				block.worldMat->trans = { -mapLeftLength + i * blockRadius * 2, -j * blockRadius * 2,0 };
				block.worldMat->scale = { blockRadius,blockRadius,blockRadius };
				block.worldMat->SetWorld();
				blocks.push_back(block);
			}
			//壊れない〃
			else if (blockNum == BLOCK::HARD && blockHardNum > 0 && blockLineNum > 0)
			{
				this->blockMapChip[j][i] = HARD;
				blockHardNum--;
				blockLineNum--;
				//登録
				Object block;
				block.worldMat->trans = { -mapLeftLength + i * blockRadius * 2, -j * blockRadius * 2,0 };
				block.worldMat->scale = { blockRadius,blockRadius,blockRadius };
				block.worldMat->SetWorld();
				blocks.push_back(block);
			}
		}
	}

}

bool Stage::CollisionMapInternal(int left, int right, int down, int up)
{
	if (blockMapChip[(int)up / (int)-(blockRadius * 2)][(int)(left + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//左上
	if (blockMapChip[(int)down / (int)-(blockRadius * 2)][(int)(left + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//左下
	if (blockMapChip[(int)up / (int)-(blockRadius * 2)][(int)(right + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//右上
	if (blockMapChip[(int)down / (int)-(blockRadius * 2)][(int)(right + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//右下

	return false;
}

void Stage::CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround)
{
	//マップチップとの判定
	//左右
	float left = pos.x - radius;
	float right = pos.x + radius;
	float down = pos.y - radius;
	float up = pos.y + radius;//移動でplayer位置が変わっている場合があるので角の更新

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//仮に進んだとしてそこにブロックがあるか
	{
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.1f, right + sign(velocity.x) * 0.1f, down, up))
		{
			pos.x += sign(velocity.x) * 0.1f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos.x - radius;
			right = pos.x + radius;
			down = pos.y - radius;
			up = pos.y + radius;//移動したので角の更新
		}

		velocity.x = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}

	pos.x += velocity.x;

	//y
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y))//仮に進んだとしてそこにブロックがあるか
	{
		if (velocity.y < 0)
		{
			isGround = true;
		}

		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.1f, up + sign(velocity.y) * 0.1f))
		{
			pos.y += sign(velocity.y) * 0.1f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos.x - radius;
			right = pos.x + radius;
			down = pos.y - radius;
			up = pos.y + radius;//移動したので角の更新
		}

		velocity.y = 0;//仮に進んだとしてそこにブロックがあれば移動停止
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
			if ((view.eye.y - blocks[count].worldMat->trans.y) < mapLeftLength)
			{
				if (blockMapChip[j][i] == NORMAL)
				{
					blocks[count].DrawModel(blocks[count].worldMat, &view, &projection, model, { 1.0f,1.0f,1.0f,1.0f });
					count++;
				}
				if (blockMapChip[j][i] == HARD)
				{
					blocks[count].DrawModel(blocks[count].worldMat, &view, &projection, model, { 1.0f,0,0,1.0f });
					count++;
				}
			}
		}
	}
}



