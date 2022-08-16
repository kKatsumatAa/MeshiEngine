#include "CollisionManager.h"
#include "Util.h"

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask())
		|| !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()))
	{
		return;//����A�Փˏ�������������
	}

	Vec3 posA = colliderA->GetWorldPos();
	Vec3 posB = colliderB->GetWorldPos();

	float rA = colliderA->GetRadius();
	float rB = colliderB->GetRadius();

	if (CollisionCircleCircle(posA, rA, posB, rB))
	{
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}


void CollisionManager::CheckCollisionPair2(Collider* colliderA, Collider* colliderB)
{
	if (colliderA->isLockOn)//player�����b�N�I�����[�h�̎��̂�
	{

		if (!(colliderA->GetCollisionAttribute() == kCollisionAttributePlayer
			&& colliderB->GetCollisionAttribute() == kCollisionAttributeEnemy)
			|| colliderB->isLockOned == true)//A��player�ŁAB���G�̎��̂ݏ���
		{
			return;//����A�Փˏ�������������
		}

		Vec3 posA = colliderA->GetWorldPos();
		Vec2 pos = Vec3toVec2(posA, view->matView, projection->matProjection);

		Vec3 nearFarPos[2];

		Vec2toNearFarPos(pos, nearFarPos[0], nearFarPos[1], view->matView, projection->matProjection);

		Vec3 posB = colliderB->GetWorldPos();

		float rA = colliderA->GetRadius();
		float rB = colliderB->GetRadius() * posB.GetLength() / 50;//��

		if (CollisionRayCircle(nearFarPos[0], nearFarPos[1], rA, posB, rB))
		{
			colliderA->OnCollision2();//���炷�p
			colliderB->OnCollision2();//�G�̂ݏ���
		}
	}
}

void CollisionManager::CheckAllCollisions()
{
	//���X�g���̃y�A�𑍓�����
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrB��itrA�̎��̗v�f����񂷁i�d�����������j
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB)
		{
			CheckCollisionPair(*itrA, *itrB);
		}
	}
}

void CollisionManager::CheckAllCollisions2()
{
	//���X�g���̃y�A�𑍓�����
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrB��itrA�̎��̗v�f����񂷁i�d�����������j
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB)
		{
			CheckCollisionPair2(*itrA, *itrB);
		}
	}
}

void CollisionManager::SetListCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::ClearList()
{
	colliders_.clear();
}
