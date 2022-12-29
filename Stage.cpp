#include "Stage.h"
#include <random>

//�����͈�
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
			//�u���b�N�ő吔�����Z�b�g
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}
		if (j % 15 == 0)
		{
			//�G�̐����Z�b�g
			enemyLineNum = enemyLineMax;
		}

		int hard[2] = { hardRand(engine),hardRand(engine) };

		for (int i = 0; i < mapNumX; i++)
		{
			int blockNum = mapRand(engine);

			//���E�̒[�͉��Ȃ��u���b�N��u��
			if ((i < hardWallNum && i >= 0) ||
				(i >= mapNumX - hardWallNum && i <= mapNumX - 1))
			{
				GenerateHardBlock(i, j);
			}
			//���Ȃ��V
			else if (((i == hardWallNum && hard[0] == HARD) || (hard[1] == HARD && i == mapNumX - hardWallNum - 1))
				&& blockHardNum > 0 && j > stageBeginNumY * 2)
			{
				blockHardNum--;
				GenerateHardBlock(i, j);
			}
			//�����u���Ȃ�
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = NONE;

				//�G����
				if (enemyRand(engine) == 0 && enemyLineNum > 0 && j > stageBeginNumY * 2)
				{
					enemyLineNum--;
					enemyM->GenerateEnemy(MapChipTransVec3(i, j));
				}
			}
			//����u���b�N
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
		//��������
		GenerateRoom();
	}
}

void Stage::GenerateBlock(int X, int Y)
{
	DeleteBlock(X, Y);

	this->blockMapChip[Y][X] = NORMAL;
	//�o�^

	blocks[Y][X] = new Object;
	blocks[Y][X]->worldMat->trans = MapChipTransVec3(X, Y);
	blocks[Y][X]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
	blocks[Y][X]->worldMat->SetWorld();
}

void Stage::GenerateHardBlock(int X, int Y)
{
	DeleteBlock(X, Y);

	this->blockMapChip[Y][X] = HARD;
	//�o�^
	blocks[Y][X] = new Object;
	blocks[Y][X]->worldMat->trans = MapChipTransVec3(X, Y);
	blocks[Y][X]->worldMat->scale = { blockRadius,blockRadius,blockRadius };
	blocks[Y][X]->worldMat->SetWorld();
}

void Stage::GenerateRoom()
{
	Room* room = new Room;

	//�����𐶐�
	int roomPosY;
	bool is = false;
	int num = 0;

	//�����̋��������ɂȂ�܂ōĒ��I
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
				//�S�ĂƔ�r����ok�Ȃ�
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
	//���ɐ���
	if (hardRand(engine) % 2 == 0) {
		roomPosX = 0;
		isleft = true;
		GenerateHardBlock(roomPosX + hardWallNum, roomPosY + 3);
		GenerateBlock(roomPosX + hardWallNum + 1, roomPosY + 3);

		room->Initialize(MapChipTransVec3(roomPosX + hardWallNum - 4, roomPosY + 1), ROOM_TYPE::LEFT_ROOM, 3.0f * blockRadius);
		rooms.push_back(room);

		//���C�g
		PointLightSetPos(lightCount, MapChipTransVec3(roomPosX + hardWallNum - 1, roomPosY));
		lightCount++;
	}
	//�E
	else
	{
		roomPosX = mapNumX - hardWallNum;
		GenerateHardBlock(roomPosX - 1, roomPosY + 3);
		GenerateBlock(roomPosX - 2, roomPosY + 3);

		room->Initialize(MapChipTransVec3(roomPosX + 2, roomPosY + 1), ROOM_TYPE::RIGHT_ROOM, 3.0f * blockRadius);
		rooms.push_back(room);

		//���C�g
		PointLightSetPos(lightCount, MapChipTransVec3(roomPosX + 1, roomPosY));
		lightCount++;
	}

	for (int j = roomPosY; j < roomPosY + 3; j++)
	{
		//���ɕ���
		if (isleft)
		{
			for (int i = roomPosX + 1; i < roomPosX + hardWallNum + 2; i++)
			{
				DeleteBlock(i, j);
			}
		}
		//�E�ɕ���
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
	//�E����
	for (int i = beginRoomY; i < beginRoomY + roomLengthY; i++)
	{
		for (int j = hardWallNum; j < hardWallNum + roomLengthX; j++)
		{
			//�V��Ə�
			if (i == beginRoomY || i == beginRoomY + roomLengthY - 1)
			{
				GenerateHardBlock(j, i);
			}
			//���ɖ߂�
			else if ((j == hardWallNum && i > beginRoomY + 1))
			{
				blockMapChip[i][j] = ROOMR;
			}
			//���E�̕�
			else if ((j == hardWallNum + roomLengthX - 1) || (j == hardWallNum && i == beginRoomY + 1))
			{
				GenerateHardBlock(j, i);
			}

		}
	}
	//���C�g
	PointLightSetPos(lightCount, MapChipTransVec3(hardWallNum + 2, beginRoomY + 2));
	lightCount++;


	//������
	for (int i = beginRoomYLeft; i < beginRoomYLeft + roomLengthY; i++)
	{
		for (int j = hardWallNum; j < hardWallNum + roomLengthX; j++)
		{
			//�V��Ə�
			if (i == beginRoomYLeft || i == beginRoomYLeft + roomLengthY - 1)
			{
				GenerateHardBlock(j, i);
			}
			//���ɖ߂�
			else if ((j == hardWallNum + roomLengthX - 1 && i > beginRoomYLeft + 1))
			{
				blockMapChip[i][j] = ROOML;
			}
			//���E�̕�
			else if ((j == hardWallNum) || (j == hardWallNum + roomLengthX - 1 && i == beginRoomYLeft + 1))
			{
				GenerateHardBlock(j, i);
			}

		}
	}

	//���C�g
	PointLightSetPos(lightCount, MapChipTransVec3(hardWallNum + 2, beginRoomYLeft + 2));
	lightCount++;
}

Vec3 Stage::MapChipTransVec3(int X, int Y)
{
	//���S���W�Ȃ̂ō�����W�ɍ��킹��iblockRadius�𑫂�����j
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
	//���o�Ȃ�

}

void Stage::PointLightSetPos(int count, Vec3 pos)
{
	//���C�g
	Vec3 posV = pos;
	XMFLOAT3 pos_ = { posV.x,posV.y,posV.z };
	lightM->SetPointLightPos(count, pos_);
}



//����
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
	}//����

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
	}//����

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
	}//�E��

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
	}//�E��

	return false;
}

void Stage::CollisionMap(Collider* collider, bool& isGround, bool& isDead, bool isBlockBreak)
{
	//�����Ƃ̔���
	CollisionRoom(collider);

	Vec3& pos = collider->GetWorldPos();
	Vec3& velocity = collider->GetVelocity();
	float radius = collider->GetRadius();

	Vec3 pos_ = { pos.x  ,pos.y  ,pos.z };

	//�}�b�v�`�b�v�Ƃ̔���
	//���E
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//�ړ���player�ʒu���ς���Ă���ꍇ������̂Ŋp�̍X�V


	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N�����邩
	{

		isDead = true;

		while (!CollisionMapInternal(left + sign(velocity.x) * 0.1f, right + sign(velocity.x) * 0.1f, down, up, isBlockBreak))
		{
			pos_.x += sign(velocity.x) * 0.1f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//�ړ������̂Ŋp�̍X�V
		}

		velocity.x = 0;//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N������Έړ���~
	}

	pos.x += velocity.x;

	//y
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y))//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N�����邩
	{
		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.1f, up + sign(velocity.y) * 0.1f, isBlockBreak))
		{
			pos_.y += sign(velocity.y) * 0.1f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//�ړ������̂Ŋp�̍X�V
		}

		isDead = true;
		if (velocity.y < 0)
		{
			isGround = true;
		}

		velocity.y = 0;//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N������Έړ���~
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

	//�}�b�v�`�b�v�Ƃ̔���
	//���E
	float left = pos.x - radius;
	float right = pos.x + radius;
	float down = pos.y - radius;
	float up = pos.y + radius;//�ړ���player�ʒu���ς���Ă���ꍇ������̂Ŋp�̍X�V

		//�����ɓ������畔���Ƀ��[�v
	if (collider->GetIsPlayer() && !isPlayerRoom)
	{
		for (Room* room : rooms)
		{
			//���̕���
			if (room->Collision(pos, radius) == ROOM_TYPE::LEFT_ROOM)
			{
				//�A�C�e������
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
			//�E�̕���
			else if (room->Collision(pos, radius) == ROOM_TYPE::RIGHT_ROOM)
			{
				//�A�C�e������
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
	//���ǂ�
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
			//�J�����O
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



