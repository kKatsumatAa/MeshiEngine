#pragma once
#include"Enemy.h"

class EnemyManager
{
private:
	void EnemyGenerate();
	Player* player;
	BulletManager* bulletManager;

	float gTimer = 300;

public:
	std::list<std::unique_ptr<Enemy>> enemies;

	void Initialize(Player* player, BulletManager* bulletManager);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//’eƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<Enemy>>& GetEnemies()
	{
		return enemies;
	}
};

