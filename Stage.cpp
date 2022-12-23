#include "Stage.h"
#include <random>

//乱数範囲
static std::uniform_int_distribution<int> mapRand(NONE, HARD + 7);
static std::uniform_int_distribution<int> hardRand(NONE, HARD + 4);
static std::uniform_int_distribution<int> roomRand(0 + Stage::stageBeginNumY, Stage::mapDownMaxNum - 10);
static std::uniform_int_distribution<int> enemyRand(0, 30);


Stage::~Stage()
{
	for (int j = 0; j < mapNumY; j++)
	{
		for (int i = 0; i < mapNumX; i++)
		{
			DeleteBlock(i, j);
		}
	}
}

void Stage::Initialize(Model* model, EnemyManager* enemyM)
{
	for (int j = 0; j < mapNumY; j++)
	{
		for (int i = 0; i < mapNumX; i++)
		{
			DeleteBlock(i, j);
		}
	}
	this->model = model;

	this->enemyM = enemyM;
}

void Stage::GenerateStage()
{
	for (int j = 0; j < mapDownMaxNum; j++)
	{
		if (j % 13 == 0)
		{
			//ブロック最大数をリセット
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}
		if (j % 15 == 0)
		{
			//敵の数リセット
			enemyLineNum = enemyLineMax;
		}

		int hard[2] = { hardRand(engine),hardRand(engine) };

		for (int i = 0; i < mapNumX; i++)
		{
			int blockNum = mapRand(engine);

			//左右の端は壊れないブロックを置く
			if ((i < hardWallNum && i >= 0) ||
				(i >= mapNumX - hardWallNum && i <= mapNumX - 1))
			{
				GenerateHardBlock(i, j);
			}
			//壊れない〃
			else if (((i == hardWallNum && hard[0] == HARD) || (hard[1] == HARD && i == mapNumX - hardWallNum - 1))
				&& blockHardNum > 0 && j > stageBeginNumY)
			{
				blockHardNum--;
				GenerateHardBlock(i, j);
			}
			//何も置かない
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = NONE;

				//敵生成
				if (enemyRand(engine) == 0 && enemyLineNum > 0 && j > stageBeginNumY)
				{
					enemyLineNum--;
					enemyM->GenerateEnemy(MapChipTransVec3(i, j));
				}
			}
			//壊れるブロック
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0 && j > stageBeginNumY)
			{
				blockLineNum--;
				GenerateBlock(i, j);
			}
		}
	}

	GenerateRoomInternal();

	for (int i = 0; i < 3; i++)
	{
		//部屋生成
		GenerateRoom();
	}
}

void Stage::GenerateBlock(int X, int Y)
{
	DeleteBlock(X, Y);

	this->blockMapChip[Y][X] = NORMAL;
	//登録

	blocks[Y][X] = new Object;
	blocks[Y][X]->worldMat->trans = MapChipTransVec3(X, Y);
	blocks[Y][X]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
	blocks[Y][X]->worldMat->SetWorld();
}

void Stage::GenerateHardBlock(int X, int Y)
{
	DeleteBlock(X, Y);

	this->blockMapChip[Y][X] = HARD;
	//登録
	blocks[Y][X] = new Object;
	blocks[Y][X]->worldMat->trans = MapChipTransVec3(X, Y);
	blocks[Y][X]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
	blocks[Y][X]->worldMat->SetWorld();
}

void Stage::GenerateRoom()
{
	//部屋を生成
	int roomPosY = roomRand(engine);
	int roomPosX;
	bool isleft = false;
	//左に生成
	if (hardRand(engine) % 2 == 0) {
		roomPosX = 0;
		isleft = true;
		GenerateHardBlock(roomPosX + hardWallNum, roomPosY + 4);
		GenerateBlock(roomPosX + hardWallNum + 1, roomPosY + 4);
	}
	//右
	else
	{
		roomPosX = mapNumX - hardWallNum;
		GenerateHardBlock(roomPosX - 1, roomPosY + 4);
		GenerateBlock(roomPosX - 2, roomPosY + 4);
	}

	for (int j = roomPosY; j < roomPosY + 4; j++)
	{
		//左に部屋
		if (isleft)
		{
			for (int i = roomPosX + 1; i < roomPosX + hardWallNum + 2; i++)
			{
				DeleteBlock(i, j);
			}
		}
		//右に部屋
		else
		{
			for (int i = roomPosX - 2; i < roomPosX + hardWallNum - 1; i++)
			{
				DeleteBlock(i, j);
			}
		}
	}
}

void Stage::GenerateRoomInternal()
{
	//右部屋
	for (int i = beginRoomY; i < beginRoomY + roomLengthY; i++)
	{
		for (int j = hardWallNum; j < hardWallNum + roomLengthX; j++)
		{
			//天井と床
			if (i == beginRoomY || i == beginRoomY + roomLengthY - 1)
			{
				GenerateHardBlock(j, i);
			}
			//左右の壁
			else if ((j == hardWallNum + roomLengthX - 1) || (j == hardWallNum && i == beginRoomY + 1))
			{
				GenerateHardBlock(j, i);
			}
			//元に戻る
			else if ((j == hardWallNum && i > beginRoomY + 1))
			{
				blockMapChip[i][j] = ROOMR;
			}
		}
	}
}

Vec3 Stage::MapChipTransVec3(int X, int Y)
{
	//中心座標なので左上座標に合わせる（blockRadiusを足したり）
	return 	{ -mapLeftLength + (float)X * blockRadius * 2.0f + blockRadius, (float)-Y * blockRadius * 2.0f - blockRadius,0 };
}

void Stage::DeleteBlock(int X, int Y)
{
	if (blocks[Y][X] != nullptr)
	{
		blockMapChip[Y][X] = NONE;

		delete blocks[Y][X];
		blocks[Y][X] = nullptr;
	}
}

void Stage::BreakBlock(const int X, const int Y)
{
	if (blockMapChip[Y][X] == NORMAL)
	{
		blockMapChip[Y][X] = NONE;
		DeleteBlock(X, Y);
	}
	//演出など

}



//判定
//----------------------------------------------------------------------------------------------------
int Stage::CollisionMapInternal(float left, float right, float down, float up, bool isBlockBreak)
{
	int X, Y;

	Y = (up / -(blockRadius * 2.0f));
	X = ((left + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		int ans = blockMapChip[Y][X];

		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return ans;
	}//左上

	Y = (down / -(blockRadius * 2.0f));
	X = ((left + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		int ans = blockMapChip[Y][X];

		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return ans;
	}//左下

	Y = (up / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		int ans = blockMapChip[Y][X];

		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return ans;
	}//右上

	Y = (down / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		int ans = blockMapChip[Y][X];

		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return ans;
	}//右下

	return false;
}

void Stage::CollisionMap(Collider* collider, bool& isGround, bool isBlockBreak)
{
	Vec3& pos = collider->GetWorldPos();
	Vec3& velocity = collider->GetVelocity();
	float radius = collider->GetRadius();

	//何故か判定おかしいので少し変えてあげる
	Vec3 pos_ = { pos.x  ,pos.y  ,pos.z };

	//マップチップとの判定
	//左右
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//移動でplayer位置が変わっている場合があるので角の更新

	{
		//部屋に入ったら部屋にワープ
		if (((pos.x > -mapLeftLength && pos.x < -mapLeftLength + hardWallNum * blockRadius * 2.0f) ||
			(pos.x < mapLeftLength && pos.x > mapLeftLength - hardWallNum * blockRadius * 2.0f))
			&& collider->GetIsPlayer() && !isPlayerRoom)
		{
			beforeRoomPos = pos - velocity;
			isPlayerRoom = true;
			pos = { MapChipTransVec3(hardWallNum + 1,beginRoomY + 4) };
		}
		else if (((CollisionMapInternal(left + velocity.x, right + velocity.x, down, up) == ROOML) ||
			(CollisionMapInternal(left + velocity.x, right + velocity.x, down, up) == ROOMR)) 
			&& isPlayerRoom)
		{
			pos = beforeRoomPos;
			isPlayerRoom = false;
		}
	}

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//仮に進んだとしてそこにブロックがあるか
	{
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.1f, right + sign(velocity.x) * 0.1f, down, up))
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
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y))//仮に進んだとしてそこにブロックがあるか
	{
		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.1f, up + sign(velocity.y) * 0.1f, isBlockBreak))
		{
			pos_.y += sign(velocity.y) * 0.1f;//1ピクセル先にブロックがなければ1ピクセル進む

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//移動したので角の更新
		}

		if (velocity.y < 0)
		{
			isGround = true;
		}

		velocity.y = 0;//仮に進んだとしてそこにブロックがあれば移動停止
	}
	else if (!CollisionMapInternal(left, right, down - 0.1f, up))
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

	for (int j = 0; j < mapNumY; j++)
	{
		for (int i = 0; i < mapNumX; i++)
		{
			//カリング
			if (blocks[j][i] != nullptr && fabsf(view.eye.y - blocks[j][i]->worldMat->trans.y) < 45.0f)
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



