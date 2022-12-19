#pragma once
#include <list>
#include "EnemyManager.h"


class ColliderManager
{
private:
	Sound* audio = nullptr;

	std::list<Collider*> colliders_;
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

public:
	void Initialize();

	void Update(Player* player, EnemyManager* enemyM, PlayerBulletManager* playerBulletM);

	//playerと敵
	void CheckAllCollisions();


	//リストに登録
	void SetListCollider(Collider* collider);
	//リストをクリア
	void ClearList();
};

