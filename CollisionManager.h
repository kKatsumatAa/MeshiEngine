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

	//���X�g�ɓo�^
	void SetListCollider(Collider* collider);
	//���X�g���N���A
	void ClearList();
};

