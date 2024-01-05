#include "EnemyManager.h"
#include "EnemyState.h"
#include "EnemyStanceState.h"
#include "ObjectManager.h"
#include "LevelManager.h"


EnemyManager& EnemyManager::GetInstance()
{
	static EnemyManager sInst;
	return sInst;
}

void EnemyManager::Initialize()
{
	waveNum_ = 0;

	//敵のウェーブの番号で判断して無効にする
	SetIsValidOtherWaveEnemy();

	waveNum_++;
}

void EnemyManager::Update()
{
	//ウェーブの敵がいなくなったら
	if (GetEnemys().size() && !GetIsAliveWaveEnemy())
	{
		waveNum_++;
	}

	//クールタイム更新
	UpdateWaveEnemyCoolTime();

	//新しいウェーブの敵の有効フラグをオン
	SetIsValidWaveEnemy();
}

void EnemyManager::UpdateWaveEnemyCoolTime()
{
	//敵のウェーブの番号で判断してクールタイム更新
	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		if (enemy->GetWaveNum() == waveNum_)
		{
			enemy->DecrementEmergeCoolTime();
		}
	}
}

void EnemyManager::Draw()
{
}


//--------------------------------------------------------------------------------------------
std::vector<IObject3D*> EnemyManager::GetEnemys()
{
	auto enemys = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "enemy2.fbx");

	return enemys;
}

Enemy* EnemyManager::TransToEnemy(IObject3D* obj)
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

		if (enemy->GetWaveNum() == waveNum_ && !enemy->GetIsValidDraw()
			&& enemy->GetEmergeCoolTime() <= 0)
		{
			enemy->SetIsValid(true);
			//出現演出のイニシャライズ
			enemy->EmergeInitialize();
		}
	}
}

bool EnemyManager::GetIsAliveWaveEnemy()
{
	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		//現在と同じウェーブ番号の敵がいたら
		if (enemy->GetWaveNum() == waveNum_ && !enemy->GetIsDead())
		{
			return true;
		}
	}

	return false;
}

bool EnemyManager::GetIsAllEnemyDead()
{
	int32_t deadEnemiesNum = 0;

	for (auto obj : GetEnemys())
	{
		Enemy* enemy = TransToEnemy(obj);

		//敵が死んでいたらカウント
		if (enemy->GetIsDead())
		{
			deadEnemiesNum++;
		}
	}

	//敵がいないか、全員死んでいたら
	if (GetEnemys().size() <= 0 || deadEnemiesNum == GetEnemys().size())
	{
		return true;
	}

	return false;
}

void EnemyManager::DrawImGui()
{
	EnemyStateAttackStance::DrawImgui();
}
