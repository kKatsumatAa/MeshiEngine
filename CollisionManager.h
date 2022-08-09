#pragma once
#include <list>
#include "Collider.h"

class CollisionManager
{
private:
	std::list<Collider*> colliders_;
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

public:
	void CheckAllCollisions();

	//リストに登録
	void SetListCollider(Collider* collider);
	//リストをクリア
	void ClearList();
};

