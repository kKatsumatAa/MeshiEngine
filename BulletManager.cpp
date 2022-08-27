#include "BulletManager.h"

void BulletManager::Initialize()
{
	enemyBullets_.clear();
}

void BulletManager::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet)
{
	enemyBullets_.push_back(std::move(enemyBulllet));
}

void BulletManager::UpdateEnemyBullet()
{
	//�e
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_)
	{
		bullet->Update();
	}

	//�e������
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet)
		{
			return bullet->IsDead();
		}
	);
}

void BulletManager::DrawEnemyBullet(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}
}
