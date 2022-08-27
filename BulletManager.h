#pragma once
#include"EnemyBullet.h"

class BulletManager
{
private:
	

public:
	//�G�̒e
	std::list< std::unique_ptr<EnemyBullet>> enemyBullets_;


	void Initialize();

	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBulllet);
	void UpdateEnemyBullet();
	void DrawEnemyBullet(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//�e���X�g���擾(const�Q��)
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets()
	{
		return enemyBullets_;
	}
};

