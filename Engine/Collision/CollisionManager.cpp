#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"
#include "MeshCollider.h"

using namespace DirectX;


CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager sInstance;
	return &sInstance;
}

bool CollisionManager::Raycast(const Ray& ray, RaycastHit* hitInfo, float maxDistance)
{
	//全属性を有効にして
	return Raycast(ray, 0xffff, hitInfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo, float maxDistance)
{
	bool result = false;
	//走査用のイテレータ
	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録するためのイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	//今までで最も近いコライダーとの交点を記録する変数
	XMVECTOR inter;

	// 全てのコライダーと総当りチェック
	it = colliders_.begin();
	for (; it != colliders_.end(); ++it) {
		BaseCollider* colA = *it;

		//属性が合わなければスキップする
		if (!(colA->attribute_ & attribute))
		{
			continue;
		}

		//球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE && colA->GetIsValid() && !colA->GetIs2D()) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//当たらなければ除外
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter)) continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance) continue;

			//今までで最も近いので記録する
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//面の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_PLANE && colA->GetIsValid() && !colA->GetIs2D()) {
			Plane* plane = dynamic_cast<Plane*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//当たらなければ除外
			if (!Collision::CheckRay2Plane(ray, *plane, &tempDistance, &tempInter)) continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance) continue;

			//今までで最も近いので記録する
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//三角の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_TRIANGLE && colA->GetIsValid() && !colA->GetIs2D()) {
			Triangle* triangle = dynamic_cast<Triangle*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//当たらなければ除外
			if (!Collision::CheckRay2Triangle(ray, *triangle, &tempDistance, &tempInter)) continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance) continue;

			//今までで最も近いので記録する
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//メッシュコライダーの場合
		if (colA->GetShapeType() == COLLISIONSHAPE_MESH && colA->GetIsValid() && !colA->GetIs2D()) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//当たらなければ除外
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter)) continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance) continue;

			//今までで最も近いので記録する
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}

	//最終的に何かにあたっていれば結果を書き込む
	if (result && hitInfo) {
		hitInfo->distance = distance;
		hitInfo->inter = inter;
		hitInfo->collider = *it_hit;
		hitInfo->object = hitInfo->collider->GetObject3d();
	}

	return result;
}

bool CollisionManager::RaycastUtil(const Vec3& endPos, const Vec3& startPos, uint16_t attribute, RaycastHit* info)
{
	Ray ray;
	Vec3 dir = endPos - startPos;
	float length = dir.GetLength();
	dir.Normalized();

	ray.dir = { dir.x_,dir.y_,dir.z_ };
	ray.start = { startPos.x_,startPos.y_,startPos.z_ };

	return Raycast(ray, attribute, info, length);
}

void CollisionManager::QuerySphere(const Sphere& sphere, QueryCallback* callBack, uint16_t attribute)
{
	assert(callBack);

	std::forward_list<BaseCollider*>::iterator it;

	//全てのコライダーと総当たりチェック
	it = colliders_.begin();
	for (; it != colliders_.end(); ++it)
	{
		BaseCollider* col = *it;

		//属性が合わなければスキップ
		if (!(col->attribute_ & attribute))
		{
			continue;
		}

		// 球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//当たってなかったら
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject)) { continue; }

			// 交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				// 戻り値がfalseの場合、継続せず終了
				return;
			}
		}
		// メッシュ
		else if (col->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//当たってなかったら
			if (!meshCollider->CheckCollisionSphere(sphere, &tempInter, &tempReject)) { continue; }

			// 交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				// 戻り値がfalseの場合、継続せず終了
				return;
			}
		}
	}
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//全ての組み合わせについて総当たりチェック
	itA = colliders_.begin();
	for (; itA != colliders_.end(); ++itA)
	{
		itB = itA;
		++itB;
		for (; itB != colliders_.end(); ++itB)
		{
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			CollisionShapeType typeA = colA->GetShapeType();
			CollisionShapeType typeB = colB->GetShapeType();

			//ともに球の場合
			if ((colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				DirectX::XMVECTOR inter;
				if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}

			//球と面の場合
			if ((typeA == COLLISIONSHAPE_PLANE || typeB == COLLISIONSHAPE_PLANE)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = nullptr;
				Plane* PlaneB = nullptr;
				DirectX::XMVECTOR inter;

				//Aが球の時
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_PLANE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					PlaneB = dynamic_cast<Plane*>(colB);
				}
				//Aが三角形の時
				else if (typeA == COLLISIONSHAPE_PLANE && typeB == COLLISIONSHAPE_SPHERE)
				{
					SphereA = dynamic_cast<Sphere*>(colB);
					PlaneB = dynamic_cast<Plane*>(colA);
				}
				if (SphereA && PlaneB &&
					Collision::CheckSphere2Plane(*SphereA, *PlaneB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}

			//球と三角形の場合
			if ((typeA == COLLISIONSHAPE_TRIANGLE || typeB == COLLISIONSHAPE_TRIANGLE)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = nullptr;
				Triangle* TriangleB = nullptr;
				DirectX::XMVECTOR inter;

				//Aが球の時
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_TRIANGLE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					TriangleB = dynamic_cast<Triangle*>(colB);
				}
				//Aが三角形の時
				else if (typeA == COLLISIONSHAPE_TRIANGLE && typeB == COLLISIONSHAPE_SPHERE)
				{
					SphereA = dynamic_cast<Sphere*>(colB);
					TriangleB = dynamic_cast<Triangle*>(colA);
				}
				if (SphereA && TriangleB &&
					Collision::CheckSphere2Triangle(*SphereA, *TriangleB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}

			//メッシュと球の判定
			if ((typeA == COLLISIONSHAPE_MESH || typeB == COLLISIONSHAPE_MESH)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = nullptr;
				MeshCollider* meshCollider = nullptr;
				DirectX::XMVECTOR inter;

				//Aが球の時
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_MESH)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					meshCollider = dynamic_cast<MeshCollider*>(colB);
				}
				//Aがメッシュコライダーの時
				else if (typeA == COLLISIONSHAPE_MESH && typeB == COLLISIONSHAPE_SPHERE)
				{
					SphereA = dynamic_cast<Sphere*>(colB);
					meshCollider = dynamic_cast<MeshCollider*>(colA);
				}
				if (SphereA && meshCollider &&
					meshCollider->CheckCollisionSphere(*SphereA, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}

		}
	}
}

void CollisionManager::Initialize()
{
	colliders_.clear();
}
