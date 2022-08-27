#pragma once
#include <list>
#include "Collider.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "WindowsApp.h"

class CollisionManager
{
private:
	ViewMat* view = nullptr;
	ProjectionMat* projection = nullptr;

	std::list<Collider*> colliders_;
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	/// <summary>
	/// ���C�̕��̓����蔻��(player�ƓG�̂�list�ɓo�^���Ďg��)
	/// </summary>
	/// <param name="colliderA"></param>
	/// <param name="colliderB"></param>
	void CheckCollisionPair2(Collider* colliderA, Collider* colliderB);

public:
	void Initialize();

	CollisionManager(ViewMat* view, ProjectionMat* projection):
		view(view),projection(projection)
	{}

	void CheckAllCollisions();
	void CheckAllCollisions2();

	//���X�g�ɓo�^
	void SetListCollider(Collider* collider);
	//���X�g���N���A
	void ClearList();
};

