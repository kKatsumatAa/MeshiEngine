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

void CollisionManager::AddCollider(BaseCollider* collider)
{
	//2D縺ｮ蝣ｴ蜷医・2D逕ｨ縺ｮ驟榊・縺ｫ霑ｽ蜉
	if (collider->GetIs2D())
	{
		colliders2D_.push_front(collider);
	}
	else
	{
		colliders3D_.push_front(collider);
	}
}

//----------------------------------------------------------------------------------------
bool CollisionManager::Raycast(const Ray& ray, RaycastHit* hitInfo, float maxDistance)
{
	//蜈ｨ螻樊ｧ繧呈怏蜉ｹ縺ｫ縺励※
	return Raycast(ray, 0xffff, hitInfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo, float maxDistance)
{
	bool result = false;
	//襍ｰ譟ｻ逕ｨ縺ｮ繧､繝・Ξ繝ｼ繧ｿ
	std::forward_list<BaseCollider*>::iterator it;
	//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞繧ｳ繝ｩ繧､繝繝ｼ繧定ｨ倬鹸縺吶ｋ縺溘ａ縺ｮ繧､繝・Ξ繝ｼ繧ｿ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞繧ｳ繝ｩ繧､繝繝ｼ縺ｮ霍晞屬繧定ｨ倬鹸縺吶ｋ螟画焚
	float distance = maxDistance;
	//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞繧ｳ繝ｩ繧､繝繝ｼ縺ｨ縺ｮ莠､轤ｹ繧定ｨ倬鹸縺吶ｋ螟画焚
	XMVECTOR inter = {};

	// 蜈ｨ縺ｦ縺ｮ3D繧ｳ繝ｩ繧､繝繝ｼ縺ｨ邱丞ｽ薙ｊ繝√ぉ繝・け(colA縺檎嶌謇・
	it = colliders3D_.begin();
	for (; it != colliders3D_.end(); ++it) {
		BaseCollider* colA = *it;

		//螻樊ｧ縺悟粋繧上↑縺代ｌ縺ｰ繧ｹ繧ｭ繝・・縺吶ｋ
		if (!(colA->attribute_ & attribute) || !colA->isValid_)
		{
			continue;
		}

		//逅・・蝣ｴ蜷・
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//蠖薙◆繧峨↑縺代ｌ縺ｰ髯､螟・
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter)) continue;
			//霍晞屬縺梧怙蟆上〒縺ｪ縺代ｌ縺ｰ髯､螟・
			if (tempDistance >= distance) continue;

			//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞縺ｮ縺ｧ險倬鹸縺吶ｋ
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//髱｢縺ｮ蝣ｴ蜷・
		if (colA->GetShapeType() == COLLISIONSHAPE_PLANE) {
			Plane* plane = dynamic_cast<Plane*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//蠖薙◆繧峨↑縺代ｌ縺ｰ髯､螟・
			if (!Collision::CheckRay2Plane(ray, *plane, &tempDistance, &tempInter)) continue;
			//霍晞屬縺梧怙蟆上〒縺ｪ縺代ｌ縺ｰ髯､螟・
			if (tempDistance >= distance) continue;

			//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞縺ｮ縺ｧ險倬鹸縺吶ｋ
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//荳芽ｧ偵・蝣ｴ蜷・
		if (colA->GetShapeType() == COLLISIONSHAPE_TRIANGLE) {
			Triangle* triangle = dynamic_cast<Triangle*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//蠖薙◆繧峨↑縺代ｌ縺ｰ髯､螟・
			if (!Collision::CheckRay2Triangle(ray, *triangle, &tempDistance, &tempInter)) continue;
			//霍晞屬縺梧怙蟆上〒縺ｪ縺代ｌ縺ｰ髯､螟・
			if (tempDistance >= distance) continue;

			//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞縺ｮ縺ｧ險倬鹸縺吶ｋ
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//繝｡繝・す繝･繧ｳ繝ｩ繧､繝繝ｼ縺ｮ蝣ｴ蜷・
		if (colA->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//蠖薙◆繧峨↑縺代ｌ縺ｰ髯､螟・
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter)) continue;
			//霍晞屬縺梧怙蟆上〒縺ｪ縺代ｌ縺ｰ髯､螟・
			if (tempDistance >= distance) continue;

			//莉翫∪縺ｧ縺ｧ譛繧りｿ代＞縺ｮ縺ｧ險倬鹸縺吶ｋ
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}

	//譛邨ら噪縺ｫ菴輔°縺ｫ縺ゅ◆縺｣縺ｦ縺・ｌ縺ｰ邨先棡繧呈嶌縺崎ｾｼ繧
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

	ray.dir = { dir.x,dir.y,dir.z };
	ray.start = { startPos.x,startPos.y,startPos.z };

	return Raycast(ray, attribute, info, length);
}


//----------------------------------------------------------------------------------------------------------
void CollisionManager::QuerySphere(const Sphere& sphere, QueryCallback* callBack, uint16_t attribute)
{
	assert(callBack);

	std::forward_list<BaseCollider*>::iterator it;

	//蜈ｨ縺ｦ縺ｮ繧ｳ繝ｩ繧､繝繝ｼ縺ｨ邱丞ｽ薙◆繧翫メ繧ｧ繝・け
	it = colliders3D_.begin();
	for (; it != colliders3D_.end(); ++it)
	{
		BaseCollider* col = *it;

		//螻樊ｧ縺悟粋繧上↑縺代ｌ縺ｰ繧ｹ繧ｭ繝・・
		if (!(col->attribute_ & attribute))
		{
			continue;
		}

		// 逅・
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//蠖薙◆縺｣縺ｦ縺ｪ縺九▲縺溘ｉ
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject)) { continue; }

			// 莠､蟾ｮ諠・ｱ繧偵そ繝・ヨ
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// 繧ｯ繧ｨ繝ｪ繝ｼ繧ｳ繝ｼ繝ｫ繝舌ャ繧ｯ蜻ｼ縺ｳ蜃ｺ縺・
			if (!callBack->OnQueryHit(info)) {
				// 謌ｻ繧雁､縺掲alse縺ｮ蝣ｴ蜷医∫ｶ咏ｶ壹○縺夂ｵゆｺ・
				return;
			}
		}
		// 繝｡繝・す繝･
		else if (col->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//蠖薙◆縺｣縺ｦ縺ｪ縺九▲縺溘ｉ
			if (!meshCollider->CheckCollisionSphere(sphere, &tempInter, &tempReject)) { continue; }

			// 莠､蟾ｮ諠・ｱ繧偵そ繝・ヨ
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// 繧ｯ繧ｨ繝ｪ繝ｼ繧ｳ繝ｼ繝ｫ繝舌ャ繧ｯ蜻ｼ縺ｳ蜃ｺ縺・
			if (!callBack->OnQueryHit(info)) {
				// 謌ｻ繧雁､縺掲alse縺ｮ蝣ｴ蜷医∫ｶ咏ｶ壹○縺夂ｵゆｺ・
				return;
			}
		}
	}
}

//-----------------------------------------------------
void CollisionManager::CheckAllCollisions()
{
	//3D縺ｮ縺吶∋縺ｦ縺ｮ蠖薙◆繧雁愛螳壹メ繧ｧ繝・け
	CheckAllCollision3D();
	//2D縺ｮ縺吶∋縺ｦ縺ｮ蠖薙◆繧雁愛螳壹メ繧ｧ繝・け
	CheckAllCollision2D();
}

//3D縺ｮ蛻､螳・
void CollisionManager::CheckAllCollision3D()
{
	std::forward_list<BaseCollider*>::iterator itrB;
	std::forward_list<BaseCollider*>::iterator itrA = colliders3D_.begin();

	for (; itrA != colliders3D_.end(); ++itrA)
	{
		itrB = itrA;
		++itrB;
		for (; itrB != colliders3D_.end(); ++itrB)
		{
			BaseCollider* colA = *itrA;
			BaseCollider* colB = *itrB;

			CollisionShapeType typeA = colA->GetShapeType();
			CollisionShapeType typeB = colB->GetShapeType();

			//縺ｨ繧ゅ↓逅・・蝣ｴ蜷・
			if ((colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
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

			//逅・→髱｢縺ｮ蝣ｴ蜷・
			if ((typeA == COLLISIONSHAPE_PLANE || typeB == COLLISIONSHAPE_PLANE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				Plane* PlaneB = nullptr;
				DirectX::XMVECTOR inter;

				//A縺檎帥縺ｮ譎・
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_PLANE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					PlaneB = dynamic_cast<Plane*>(colB);
				}
				//A縺御ｸ芽ｧ貞ｽ｢縺ｮ譎・
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

			//逅・→荳芽ｧ貞ｽ｢縺ｮ蝣ｴ蜷・
			if ((typeA == COLLISIONSHAPE_TRIANGLE || typeB == COLLISIONSHAPE_TRIANGLE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				Triangle* TriangleB = nullptr;
				DirectX::XMVECTOR inter;

				//A縺檎帥縺ｮ譎・
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_TRIANGLE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					TriangleB = dynamic_cast<Triangle*>(colB);
				}
				//A縺御ｸ芽ｧ貞ｽ｢縺ｮ譎・
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

			//繝｡繝・す繝･縺ｨ逅・・蛻､螳・
			if ((typeA == COLLISIONSHAPE_MESH || typeB == COLLISIONSHAPE_MESH)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				MeshCollider* meshCollider = nullptr;
				DirectX::XMVECTOR inter;

				//A縺檎帥縺ｮ譎・
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_MESH)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					meshCollider = dynamic_cast<MeshCollider*>(colB);
				}
				//A縺後Γ繝・す繝･繧ｳ繝ｩ繧､繝繝ｼ縺ｮ譎・
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

void CollisionManager::CheckAllCollision2D()
{
	std::forward_list<BaseCollider*>::iterator itrB;
	std::forward_list<BaseCollider*>::iterator itrA = colliders2D_.begin();

	//2D縺ｮ縺ｿ
	for (; itrA != colliders2D_.end(); ++itrA)
	{
		itrB = itrA;
		++itrB;
		for (; itrB != colliders2D_.end(); ++itrB)
		{
			BaseCollider* colA = *itrA;
			BaseCollider* colB = *itrB;

			CollisionShapeType typeA = colA->GetShapeType();
			CollisionShapeType typeB = colB->GetShapeType();

			//縺ｨ繧ゅ↓蜀・・蝣ｴ蜷・
			if ((typeA == COLLISIONSHAPE_CIRCLE &&
				typeB == COLLISIONSHAPE_CIRCLE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Circle* CircleA = dynamic_cast<Circle*>(colA);
				Circle* CircleB = dynamic_cast<Circle*>(colB);
				DirectX::XMVECTOR inter;
				if (Collision::CheckCircle2Circle(*CircleA, *CircleB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
		}
	}
}

//-----------------------------------------------------------------
void CollisionManager::Initialize()
{
	colliders3D_.clear();
}
