#include "BulletManager.h"

void BulletManager::Initialize(ParticleManager* pManager)
{
	particleManager = pManager;
	enemyBullets_.clear();
}

void BulletManager::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet)
{
	enemyBullets_.push_back(std::move(enemyBulllet));
}

void BulletManager::UpdateEnemyBullet()
{
	//íe
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_)
	{
		bullet->Update();

		if (bullet->isDead)
		{
			particleManager->GenerateRandomParticle(bullet->worldMat.trans, 30, bullet->worldMat.scale.x * 0.5f,
				{ 0.5f,0.5f,0.5f,1.0f }, 4);
		}
	}

	//íeÇè¡Ç∑
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
