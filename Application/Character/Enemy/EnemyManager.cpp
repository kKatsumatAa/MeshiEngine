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

	//敵のウェーブの番号で判断して無効にする
	SetIsValidOtherWaveEnemy();
}

void EnemyManager::Update()
{
	//ウェーブの敵がいなくなったら
	if (GetEnemys().size() && !GetIsAliveWaveEnemy())
	{
		waveNum_++;

		//新しいウェーブの敵の有効フラグをオン
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
	//敵のウェーブの番号で判断して無効にする
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
	//敵のウェーブの番号で判断して有効にする
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

		//現在と同じウェーブ番号の敵がいたら
		if (enemy->GetWaveNum() == waveNum_)
		{
			return true;
		}
	}

	return false;
}
