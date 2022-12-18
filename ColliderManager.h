#pragma once
#include <list>
#include "Player.h"


class ColliderManager
{
private:
	Sound* audio = nullptr;

	std::list<Collider*> colliders_;
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

public:
	void Initialize();

	void Update(Player* player/*,Enemy* enemy*/);

	//player�ƓG
	void CheckAllCollisions();


	//���X�g�ɓo�^
	void SetListCollider(Collider* collider);
	//���X�g���N���A
	void ClearList();
};

