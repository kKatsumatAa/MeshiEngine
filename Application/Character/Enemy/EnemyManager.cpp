#include "EnemyManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"


EnemyManager& EnemyManager::GetInstance()
{
	static EnemyManager sInst;
	return sInst;
}

void EnemyManager::Initialize()
{
	waveNum_ = 1;

	//�G�̃E�F�[�u�̔ԍ��Ŕ��f���Ė����ɂ���
	SetIsValidOtherWaveEnemy();
}

void EnemyManager::Update()
{
	//�E�F�[�u�̓G�����Ȃ��Ȃ�����
	if (GetEnemys().size() && !GetIsAliveWaveEnemy())
	{
		waveNum_++;

		//�V�����E�F�[�u�̓G�̗L���t���O���I��
		SetIsValidWaveEnemy();
	}
}

void EnemyManager::Draw()
{
}


//--------------------------------------------------------------------------------------------
std::vector<Object*> EnemyManager::GetEnemys()
{
	auto enemys = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "enemy2.fbx");

	return enemys;
}

Enemy* EnemyManager::TransToEnemy(Object* obj)
{
	return dynamic_cast<Enemy*>(obj);
}

void EnemyManager::SetIsValidOtherWaveEnemy()
{
	//�G�̃E�F�[�u�̔ԍ��Ŕ��f���Ė����ɂ���
	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		if (enemy->GetWaveNum() != waveNum_)
		{
			enemy->SetIsValid(false);
		}
	}
}

void EnemyManager::SetIsValidWaveEnemy()
{
	//�G�̃E�F�[�u�̔ԍ��Ŕ��f���ėL���ɂ���
	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		if (enemy->GetWaveNum() == waveNum_)
		{
			enemy->SetIsValid(true);
		}
	}
}

bool EnemyManager::GetIsAliveWaveEnemy()
{
	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		//���݂Ɠ����E�F�[�u�ԍ��̓G��������
		if (enemy->GetWaveNum() == waveNum_)
		{
			return true;
		}
	}

	return false;
}
