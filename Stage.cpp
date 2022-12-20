#include "Stage.h"
#include <random>

//�����V�[�h������
static std::random_device seed_gen;
//�����Z���k�c�C�X�^�[
static std::mt19937_64 engine(seed_gen());
//�����͈�
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
			//�u���b�N�ő吔�����Z�b�g
			blockHardNum = blockHardNumMax;
			blockLineNum = blockLineNumMax;
		}

		for (int i = 0; i < mapLengthX; i++)
		{
			int blockNum = mapRand(engine);


			//�����u���Ȃ�
			if (blockNum != BLOCK::NORMAL && blockNum != BLOCK::HARD)
			{
				this->blockMapChip[j][i] = 0;
			}
			//����u���b�N
			else if (blockNum == BLOCK::NORMAL && blockLineNum > 0)
			{
				this->blockMapChip[j][i] = NORMAL;
				blockLineNum--;
				//�o�^
				Object block;
				block.worldMat->trans = { -mapLeftLength + i * blockRadius * 2, -j * blockRadius * 2,0 };
				block.worldMat->scale = { blockRadius,blockRadius,blockRadius };
				block.worldMat->SetWorld();
				blocks.push_back(block);
			}
			//���Ȃ��V
			else if (blockNum == BLOCK::HARD && blockHardNum > 0 && blockLineNum > 0)
			{
				this->blockMapChip[j][i] = HARD;
				blockHardNum--;
				blockLineNum--;
				//�o�^
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
	if (blockMapChip[(int)up / (int)-(blockRadius * 2)][(int)(left + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//����
	if (blockMapChip[(int)down / (int)-(blockRadius * 2)][(int)(left + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//����
	if (blockMapChip[(int)up / (int)-(blockRadius * 2)][(int)(right + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//�E��
	if (blockMapChip[(int)down / (int)-(blockRadius * 2)][(int)(right + mapLeftLength) / (int)(blockRadius * 2)] != 0) { return TRUE; }//�E��

	return false;
}

void Stage::CollisionMap(Vec3& pos, Vec3& velocity, float radius, bool& isGround)
{
	//�}�b�v�`�b�v�Ƃ̔���
	//���E
	float left = pos.x - radius;
	float right = pos.x + radius;
	float down = pos.y - radius;
	float up = pos.y + radius;//�ړ���player�ʒu���ς���Ă���ꍇ������̂Ŋp�̍X�V

	if (CollisionMapInternal(left + velocity.x, right + velocity.x, down, up))//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N�����邩
	{
		while (!CollisionMapInternal(left + sign(velocity.x) * 0.1f, right + sign(velocity.x) * 0.1f, down, up))
		{
			pos.x += sign(velocity.x) * 0.1f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

			left = pos.x - radius;
			right = pos.x + radius;
			down = pos.y - radius;
			up = pos.y + radius;//�ړ������̂Ŋp�̍X�V
		}

		velocity.x = 0;//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N������Έړ���~
	}

	pos.x += velocity.x;

	//y
	if (CollisionMapInternal(left, right, down + velocity.y, up + velocity.y))//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N�����邩
	{
		if (velocity.y < 0)
		{
			isGround = true;
		}

		while (!CollisionMapInternal(left, right, down + sign(velocity.y) * 0.1f, up + sign(velocity.y) * 0.1f))
		{
			pos.y += sign(velocity.y) * 0.1f;//1�s�N�Z����Ƀu���b�N���Ȃ����1�s�N�Z���i��

			left = pos.x - radius;
			right = pos.x + radius;
			down = pos.y - radius;
			up = pos.y + radius;//�ړ������̂Ŋp�̍X�V
		}

		velocity.y = 0;//���ɐi�񂾂Ƃ��Ă����Ƀu���b�N������Έړ���~
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



