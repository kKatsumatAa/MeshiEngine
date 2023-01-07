#pragma once
#include"Enemy.h"


class EnemyManager
{
private:
	Model** model = nullptr;
	//sound
	Sound* audio = nullptr;

	Player* player;

public:
	//�G�̒e
	std::list< std::unique_ptr<Enemy>> enemies;


	void Initialize(Model** model, Player* player);

	void GenerateEnemy(const Vec3& position);
	void GenerateEnemy2(const Vec3& position);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//�e���X�g���擾(const�Q��)
	const std::list<std::unique_ptr<Enemy>>& GetEnemies()
	{
		return enemies;
	}
};

