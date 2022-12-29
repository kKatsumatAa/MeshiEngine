#include "Stage.h"
#include <random>

//乱数範囲
static std::uniform_int_distribution<int> mapRand(NONE, HARD + 7);
static std::uniform_int_distribution<int> hardRand(NONE, HARD + 5);
static std::uniform_int_distribution<int> roomRand(0 + Stage::stageBeginNumY * 2, Stage::mapDownMaxNum - 10);
static std::uniform_int_distribution<int> enemyRand(0, 30);
static std::uniform_int_distribution<int> itemRand(ITEM_TYPE::INORMAL, ITEM_TYPE::ISHOTGUN);


Stage::~Stage()
{

	for (int j = 0; j < mapNumY; j++)
	{
		for (int i = 0; i < mapNumX; i++)
		{
			DeleteBlock(i, j);
		}
	}
	rooms.clear();
	roomNumY.clear();
}

void Stage::Initialize(Model* model, EnemyManager* enemyM, ItemManager* itemM, LightManager* lightM)
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
	this->itemM = itemM;
	this->lightM = lightM;
	rooms.clear();
	roomNumY.clear();
	lightCount = 1;
}

void Stage::GenerateStage()
{
	for (int j = stageBeginNumY; j < mapDownMaxNum; j++)
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
				&& blockHardNum > 0 && j > stageBeginNumY * 2)
			{
				blockHardNum--;
				GenerateHardBlock(i, j);
			}
			//何も置かない
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = NONE;

				//敵生成
				if (enemyRand(engine) == 0 && enemyLineNum > 0 && j > stageBeginNumY * 2)
				{
					enemyLineNum--;
					enemyM->GenerateEnemy(MapChipTransVec3(i, j));
				}
			}
			//壊れるブロック
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0 && j > stageBeginNumY * 2)
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
	Room* room = new Room;

	//部屋を生成
	int roomPosY;
	bool is = false;
	int num = 0;

	//部屋の距離が一定になるまで再抽選
	while (!is)
	{
		roomPosY = roomRand(engine);

		if (roomNumY.size() == 0)
		{
			roomNumY.push_back(roomPosY);
			break;
		}

		for (int i = 0; i < roomNumY.size(); i++)
		{
			if (fabsf(roomPosY - roomNumY[i]) >= 20)
			{
				num++;
				//全てと比較してokなら
				if (num >= roomNumY.size())
				{
					roomNumY.push_back(roomPosY);
					is = true;
				}
			}
		}

	}

	int roomPosX;
	bool isleft = false;
	//左に生成
	if (hardRand(engine) % 2 == 0) {
		roomPosX = 0;
		isleft = true;
		GenerateHardBlock(roomPosX + hardWallNum, roomPosY + 3);
		GenerateBlock(roomPosX + hardWallNum + 1, roomPosY + 3);

		room->Initialize(MapChipTransVec3(roomPosX + hardWallNum - 4, roomPosY + 1), ROOM_TYPE::LEFT_ROOM, 3.0f * blockRadius);
		rooms.push_back(room);

		//ライト
		PointLightSetPos(lightCount, MapChipTransVec3(roomPosX + hardWallNum - 1, roomPosY));
		lightCount++;
	}
	//右
	else
	{
		roomPosX = mapNumX - hardWallNum;
		GenerateHardBlock(roomPosX - 1, roomPosY + 3);
		GenerateBlock(roomPosX - 2, roomPosY + 3);

		room->Initialize(MapChipTransVec3(roomPosX + 2, roomPosY + 1), ROOM_TYPE::RIGHT_ROOM, 3.0f * blockRadius);
		rooms.push_back(room);

		//ライト
		PointLightSetPos(lightCount, MapChipTransVec3(roomPosX + 1, roomPosY));
		lightCount++;
	}

	for (int j = roomPosY; j < roomPosY + 3; j++)
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
			//元に戻る
			else if ((j == hardWallNum && i > beginRoomY + 1))
			{
				blockMapChip[i][j] = ROOMR;
			}
			//左右の壁
			else if ((j == hardWallNum + roomLengthX - 1) || (j == hardWallNum && i == beginRoomY + 1))
			{
				GenerateHardBlock(j, i);
			}

		}
	}
	//ライト
	PointLightSetPos(lightCount, MapChipTransVec3(hardWallNum + 2, beginRoomY + 2));
	lightCount++;


	//左部屋
	for (int i = beginRoomYLeft; i < beginRoomYLeft + roomLengthY; i++)
	{
		for (int j = hardWallNum; j < hardWallNum + roomLengthX; j++)
		{
			//天井と床
			if (i == beginRoomYLeft || i == beginRoomYLeft + roomLengthY - 1)
			{
				GenerateHardBlock(j, i);
			}
			//元に戻る
			else if ((j == hardWallNum + roomLengthX - 1 && i > beginRoomYLeft + 1))
			{
				blockMapChip[i][j] = ROOML;
			}
			//左右の壁
			else if ((j == hardWallNum) || (j == hardWallNum + roomLengthX - 1 && i == beginRoomYLeft + 1))
			{
				GenerateHardBlock(j, i);
			}

		}
	}

	//ライト
	PointLightSetPos(lightCount, MapChipTransVec3(hardWallNum + 2, beginRoomYLeft + 2));
	lightCount++;
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

void Stage::PointLightSetPos(int count, Vec3 pos)
{
	//ライト
	Vec3 posV = pos;
	XMFLOAT3 pos_ = { posV.x,posV.y,posV.z };
	lightM->SetPointLightPos(count, pos_);
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

void Stage::CollisionMap(Collider* collider, bool& isGround, bool& isDead, bool isBlockBreak)
{
	//部屋との判定
	CollisionRoom(collider);

	Vec3& pos = collider->GetWorldPos();
	Vec3& velocity = collider->GetVelocity();
	float radius = collider->GetRadius();

	Vec3 pos_ = { pos.x  ,pos.y  ,pos.z };

	//マップチップとの判定
	//左右
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//移動でplayer位置が変わっている場合があるので角の更新


	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//仮に進んだとしてそこにブロックがあるか
	{

		isDead = true;

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

		isDead = true;
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

void Stage::CollisionRoom(Collider* collider)
{
	Vec3& pos = collider->GetWorldPos();
	Vec3& velocity = collider->GetVelocity();
	float radius = collider->GetRadius();

	//マップチップとの判定
	//左右
	float left = pos.x - radius;
	float right = pos.x + radius;
	float down = pos.y - radius;
	float up = pos.y + radius;//移動でplayer位置が変わっている場合があるので角の更新

		//部屋に入ったら部屋にワープ
	if (collider->GetIsPlayer() && !isPlayerRoom)
	{
		for (Room* room : rooms)
		{
			//左の部屋
			if (room->Collision(pos, radius) == ROOM_TYPE::LEFT_ROOM)
			{
				//アイテム生成
				if (!room->GetIsUsed())
				{
					itemM->items_.clear();

					itemM->GenerateItem(MapChipTransVec3(hardWallNum + roomLengthX - 6, beginRoomYLeft + 5), itemRand(engine));
				}
				room->SetIsUsed(true);

				beforeRoomPos = pos + Vec3(1.0f, 0, 0);
				isPlayerRoom = true;
				pos = { MapChipTransVec3(hardWallNum + roomLengthX - 2,beginRoomYLeft + 5) };

			}
			//右の部屋
			else if (room->Collision(pos, radius) == ROOM_TYPE::RIGHT_ROOM)
			{
				//アイテム生成
				if (!room->GetIsUsed())
				{
					itemM->items_.clear();

					itemM->GenerateItem(MapChipTransVec3(hardWallNum + 6, beginRoomY + 5), itemRand(engine));
				}
				room->SetIsUsed(true);

				beforeRoomPos = pos + Vec3(-1.0f, 0, 0);
				isPlayerRoom = true;
				pos = { MapChipTransVec3(hardWallNum + 1,beginRoomY + 5) };

			}
		}
	}
	//もどる
	if (((CollisionMapInternal(left + velocity.x, right + velocity.x, down, up) == ROOML) ||
		(CollisionMapInternal(left + velocity.x, right + velocity.x, down, up) == ROOMR))
		/*&& isPlayerRoom*/)
	{
		pos = beforeRoomPos;
		velocity = { 0,0,0 };
		isPlayerRoom = false;
	}

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



