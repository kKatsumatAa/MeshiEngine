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

	//playerと敵
	void CheckAllCollisions();


	//リストに登録
	void SetListCollider(Collider* collider);
	//リストをクリア
	void ClearList();
};

