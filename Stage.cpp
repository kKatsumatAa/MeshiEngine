#include "Stage.h"
#include <random>

//�����V�[�h������
static std::random_device seed_gen;
//�����Z���k�c�C�X�^�[
static std::mt19937_64 engine(seed_gen());
//�����͈�
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
			//�u���b�N�ő吔�����Z�b�g
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}

		int hard[2] = { hardRand(engine),hardRand(engine) };

		for (int i = 0; i < mapLengthX; i++)
		{
			int blockNum = mapRand(engine);

			//���E�̒[�͉��Ȃ��u���b�N��u��
			if (i == 0 || i == mapLengthX - 1)
			{
				this->blockMapChip[j][i] = HARD;
				GenerateBlock(i, j);
			}
			//���Ȃ��V
			else if (((i == 1 && hard[0] == HARD) || (hard[1] == HARD && i == mapLengthX - 2))
				&& blockHardNum > 0 && j > 10)
			{
				this->blockMapChip[j][i] = HARD;
				blockHardNum--;
				blockLineNum--;

				GenerateBlock(i, j);
			}
			//�����u���Ȃ�
			else if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = 0;
			}
			//����u���b�N
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
	//�o�^
	blocks[Y][X] = new Object;                                                     //���S���W�Ȃ̂ō�����W�ɍ��킹��iblockRadius�𑫂�����j
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
	//���o�Ȃ�

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

void Stage::CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround, bool isBlockBreak)
{
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
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.01f, right + sign(velocity.x) * 0.01f, down, up, isBlockBreak))
		{
			pos_.x += sign(velocity.x) * 0.01f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

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
		if (CollisionMapInternal(left, right, down - 1.0f, up))
		{
			isGround = true;
		}

		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.01f, up + sign(velocity.y) * 0.01f, isBlockBreak))
		{
			pos_.y += sign(velocity.y) * 0.01f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

			left = pos_.x - radius;
			right = pos_.x + radius;
			down = pos_.y - radius;
			up = pos_.y + radius;//�ړ������̂Ŋp�̍X�V
		}

		velocity.y = 0;//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N������Έړ���~
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



