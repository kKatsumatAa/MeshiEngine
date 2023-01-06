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
	//球を生成、初期化
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(model, 0.5f, player);
	enemy.get()->SetWorldPos(position);
	//球を登録
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Update();

		if (enemy->GetIsDead())
		{
			//パーティクル
			ParticleManager::GetInstance()->GenerateRandomParticle(10, 20, 1.0f, enemy->GetWorldPos(),
				2.5f, 0.0f, { 1.0f,0,0,1.0f }, { 0,0,0,0 });
		}
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
