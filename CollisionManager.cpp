#include "CollisionManager.h"
#include "Util.h"

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask())
		|| !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()))
	{
		return;//判定、衝突処理せず抜ける
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
	if (colliderA->isLockOn)//playerがロックオンモードの時のみ
	{

		if (!(colliderA->GetCollisionAttribute() == kCollisionAttributePlayer
			&& colliderB->GetCollisionAttribute() == kCollisionAttributeEnemy)
			|| colliderB->isLockOned == true)//Aがplayerで、Bが敵の時のみ処理
		{
			return;//判定、衝突処理せず抜ける
		}

		Vec3 posA = colliderA->GetWorldPos();
		Vec2 pos = Vec3toVec2(posA, view->matView, projection->matProjection);

		Vec3 nearFarPos[2];

		Vec2toNearFarPos(pos, nearFarPos[0], nearFarPos[1], view->matView, projection->matProjection);

		Vec3 posB = colliderB->GetWorldPos();

		float rA = colliderA->GetRadius();
		float rB = colliderB->GetRadius() * posB.GetLength() / 50;//仮

		if (CollisionRayCircle(nearFarPos[0], nearFarPos[1], rA, posB, rB))
		{
			colliderA->OnCollision2();//音鳴らす用
			colliderB->OnCollision2();//敵のみ処理
		}
	}
}

void CollisionManager::CheckAllCollisions()
{
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrBはitrAの次の要素から回す（重複判定を回避）
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
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrBはitrAの次の要素から回す（重複判定を回避）
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
