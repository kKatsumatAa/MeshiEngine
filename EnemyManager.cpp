#include "EnemyManager.h"

void EnemyManager::Initialize(Model* model, Player* player)
{
	assert(model);

	this->model = model;
	this->player = player;
	enemies.clear();
}

void EnemyManager::GenerateEnemy(const Vec3& position)
{
	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(model, player);
	enemy.get()->SetWorldPos(position);
	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Update();
	}

	enemies.remove_if([](std::unique_ptr<Enemy>& enemy)
		{
			return enemy->GetIsDead();
		}
	);
}

void EnemyManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		if (fabs(view.eye.y - enemy->GetWorldPos().y) < 45.0f)
		{
			enemy->Draw(view, projection);
		}
	}
}
