#pragma once
#include"EnemyBullet.h"
#include"Particle.h"

class BulletManager
{
private:
	ParticleManager* particleManager = nullptr;

public:
	//敵の弾
	std::list< std::unique_ptr<EnemyBullet>> enemyBullets_;


	void Initialize(ParticleManager* pManager);

	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet);
	void UpdateEnemyBullet();
	void DrawEnemyBullet(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets()
	{
		return enemyBullets_;
	}
};

