#pragma once
#include"EnemyBullet.h"

class BulletManager
{
private:
	

public:
	//“G‚Ì’e
	std::list< std::unique_ptr<EnemyBullet>> enemyBullets_;


	void Initialize();

	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet);
	void UpdateEnemyBullet();
	void DrawEnemyBullet(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//’eƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets()
	{
		return enemyBullets_;
	}
};

