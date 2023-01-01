#pragma once
#include <list>
#include "EnemyManager.h"
#include "Stage.h"


class ColliderManager
{
private:
	Sound* audio = nullptr;

	std::list<Collider*> colliders_;
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	const float shakeLength = 1.2f;
	const int shakeTime = 8;

public:
	void Initialize();

	void Update(Player* player, EnemyManager* enemyM, PlayerBulletManager* playerBulletM, ItemManager* itemM, Stage* stage,
		Camera* camera);

	//player�ƓG
	void CheckAllCollisions();


	//���X�g�ɓo�^
	void SetListCollider(Collider* collider);
	//���X�g���N���A
	void ClearList();
};

