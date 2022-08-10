#include "EnemyManager.h"
#include <random>


void EnemyManager::Initialize(Player* player, BulletManager* bulletManager)
{
	this->player = player;
	this->bulletManager = bulletManager;
}

void EnemyManager::EnemyGenerate()
{
	//�����V�[�h������
	std::random_device seed_gen;
	//�����Z���k�c�C�X�^�[
	std::mt19937_64 engine(seed_gen());
	//�����͈�
	std::uniform_real_distribution<float> posDistY(-playerMoveRange.y, playerMoveRange.y);
	std::uniform_real_distribution<float> posDistX(-playerMoveRange.x, playerMoveRange.x);

	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(player, bulletManager,{ posDistX(engine) ,posDistY(engine) ,200.0f});
	/*enemy->SetPlayer(player_);*/
	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	gTimer++;

	if (gTimer >= 300.0f)
	{
		EnemyGenerate();
		gTimer = 0;
	}

	//�e
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Update();
	}

	//�e������
	enemies.remove_if([](std::unique_ptr<Enemy>& enemy)
		{
			return enemy->IsDead();
		}
	);
}

void EnemyManager::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Draw(view, projection, texHundle);
	}
}
