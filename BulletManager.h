#pragma once
#include"EnemyBullet.h"

class BulletManager
{
private:
	

public:
	//敵の弾
	std::list< std::unique_ptr<EnemyBullet>> enemyBullets_;


	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet);
	void UpdateEnemyBullet();
	void DrawEnemyBullet(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets()
	{
		return enemyBullets_;
	}
};

