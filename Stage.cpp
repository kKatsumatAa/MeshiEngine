#include "Stage.h"
#include <random>

//�����͈�
static std::uniform_int_distribution<int> mapRand(NONE, HARD + 7);
static std::uniform_int_distribution<int> hardRand(NONE, HARD + 4);
static std::uniform_int_distribution<int> roomRand(0 + Stage::stageBeginY, Stage::mapLengthY - 10);
static std::uniform_int_distribution<int> enemyRand(0, 30);


Stage::~Stage()
{
	for (int j = 0; j < mapLengthY; j++)
	{
		for (int i = 0; i < mapLengthX; i++)
		{
			DeleteBlock(i, j);
		}
	}
}

void Stage::Initialize(Model* model, EnemyManager* enemyM)
{
	for (int j = 0; j < mapLengthY; j++)
	{
		for (int i = 0; i < mapLengthX; i++)
		{
			DeleteBlock(i, j);
		}
	}
	this->model = model;

	this->enemyM = enemyM;
}

void Stage::GenerateStage()
{
	for (int j = 0; j < mapLengthY; j++)
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

		for (int i = 0; i < mapLengthX; i++)
		{
			int blockNum = mapRand(engine);

			//���E�̒[�͉��Ȃ��u���b�N��u��
			if ((i < hardWallLength && i >= 0) ||
				(i >= mapLengthX - hardWallLength && i <= mapLengthX - 1))
			{
				GenerateHardBlock(i, j);
			}
			//���Ȃ��V
			else if (((i == hardWallLength && hard[0] == HARD) || (hard[1] == HARD && i == mapLengthX - hardWallLength - 1))
				&& blockHardNum > 0 && j > stageBeginY)
			{
				blockHardNum--;
				GenerateHardBlock(i, j);
			}
			//�����u���Ȃ�
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = NONE;

				//�G����
				if (enemyRand(engine) == 0 && enemyLineNum > 0 && j > stageBeginY)
				{
					enemyLineNum--;
					enemyM->GenerateEnemy(MapChipTransVec3(i, j));
				}
			}
			//����u���b�N
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0 && j > stageBeginY)
			{
				blockLineNum--;
				GenerateBlock(i, j);
			}
		}
	}

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
	//�����𐶐�
	int roomPosY = roomRand(engine);
	int roomPosX;
	bool isleft = false;
	//���ɐ���
	if (hardRand(engine) % 2 == 0) {
		roomPosX = 0;
		isleft = true;
		GenerateHardBlock(roomPosX + hardWallLength, roomPosY + 4);
		GenerateBlock(roomPosX + hardWallLength + 1, roomPosY + 4);
	}
	//�E
	else
	{
		roomPosX = mapLengthX - hardWallLength;
		GenerateHardBlock(roomPosX - 1, roomPosY + 4);
		GenerateBlock(roomPosX - 2, roomPosY + 4);
	}

	for (int j = roomPosY; j < roomPosY + 4; j++)
	{
		//���ɕ���
		if (isleft)
		{
			for (int i = roomPosX + 1; i < roomPosX + hardWallLength + 2; i++)
			{
				DeleteBlock(i, j);
			}
		}
		//�E�ɕ���
		else
		{
			for (int i = roomPosX - 2; i < roomPosX + hardWallLength - 1; i++)
			{
				DeleteBlock(i, j);
			}
		}
	}
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
	if (blockMapChip[Y][X] == NORMAL) {
		blockMapChip[Y][X] = NONE;
		DeleteBlock(X, Y);
	}
	//���o�Ȃ�

}



//����
//----------------------------------------------------------------------------------------------------
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
	}//����

	Y = (down / -(blockRadius * 2.0f));
	X = ((left + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//����

	Y = (up / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//�E��

	Y = (down / -(blockRadius * 2.0f));
	X = ((right + (float)mapLeftLength) / (blockRadius * 2.0f));
	if (blockMapChip[Y][X] != 0)
	{
		if (isBlockBreak)
		{
			BreakBlock(X, Y);
		}
		return TRUE;
	}//�E��

	return false;
}

void Stage::CollisionMap(Collider* collider, bool& isGround, bool isBlockBreak)
{
	Vec3& pos = collider->GetWorldPos();
	Vec3& velocity = collider->GetVelocity();
	float radius = collider->GetRadius();

	//���̂����肨�������̂ŏ����ς��Ă�����
	Vec3 pos_ = { pos.x  ,pos.y  ,pos.z };

	//�}�b�v�`�b�v�Ƃ̔���
	//���E
	float left = pos_.x - radius;
	float right = pos_.x + radius;
	float down = pos_.y - radius;
	float up = pos_.y + radius;//�ړ���player�ʒu���ς���Ă���ꍇ������̂Ŋp�̍X�V

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N�����邩
	{
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


	//�����ɓ����Ă�����t���OTRUE
	if ((pos.x > -mapLeftLength && pos.x < -mapLeftLength + hardWallLength * blockRadius * 2.0f) ||
		(pos.x < mapLeftLength && pos.x > mapLeftLength - hardWallLength * blockRadius * 2.0f) && collider->GetIsPlayer())
	{
		isPlayerRoom = true;
	}
	else if(collider->GetIsPlayer())
	{
		isPlayerRoom = false;
	}
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

			////������x���ɍs������폜����
			//if ((view.eye.y) + 50.0f < blocks[j][i]->worldMat->trans.y)
			//{
			//	DeleteBlock(i, j);
			//}
		}
	}
}



