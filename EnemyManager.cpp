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
	//‹…‚ğ¶¬A‰Šú‰»
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(model, player);
	enemy.get()->SetWorldPos(position);
	//‹…‚ğ“o˜^
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


	//‰¼
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_2))
	{
		for (int i = 0; i < 3; i++)
		{
			GenerateEnemy({ player->GetWorldPos().x + posDistX(engine), player->GetWorldPos().y - 20.0f + posDistY(engine), 0 });
		}
	}
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
