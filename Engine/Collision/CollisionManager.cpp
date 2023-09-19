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
	//2D�̏ꍇ��2D�p�̔z��ɒǉ�
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
	//�S������L���ɂ���
	return Raycast(ray, 0xffff, hitInfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo, float maxDistance)
{
	bool result = false;
	//�����p�̃C�e���[�^
	std::forward_list<BaseCollider*>::iterator it;
	//���܂łōł��߂��R���C�_�[���L�^���邽�߂̃C�e���[�^
	std::forward_list<BaseCollider*>::iterator it_hit;
	//���܂łōł��߂��R���C�_�[�̋������L�^����ϐ�
	float distance = maxDistance;
	//���܂łōł��߂��R���C�_�[�Ƃ̌�_���L�^����ϐ�
	XMVECTOR inter;

	// �S�Ă�3D�R���C�_�[�Ƒ�����`�F�b�N(colA������)
	it = colliders3D_.begin();
	for (; it != colliders3D_.end(); ++it) {
		BaseCollider* colA = *it;

		//����������Ȃ���΃X�L�b�v����
		if (!(colA->attribute_ & attribute) || !colA->isValid_)
		{
			continue;
		}

		//���̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//������Ȃ���Ώ��O
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter)) continue;
			//�������ŏ��łȂ���Ώ��O
			if (tempDistance >= distance) continue;

			//���܂łōł��߂��̂ŋL�^����
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//�ʂ̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_PLANE) {
			Plane* plane = dynamic_cast<Plane*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//������Ȃ���Ώ��O
			if (!Collision::CheckRay2Plane(ray, *plane, &tempDistance, &tempInter)) continue;
			//�������ŏ��łȂ���Ώ��O
			if (tempDistance >= distance) continue;

			//���܂łōł��߂��̂ŋL�^����
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//�O�p�̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_TRIANGLE) {
			Triangle* triangle = dynamic_cast<Triangle*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//������Ȃ���Ώ��O
			if (!Collision::CheckRay2Triangle(ray, *triangle, &tempDistance, &tempInter)) continue;
			//�������ŏ��łȂ���Ώ��O
			if (tempDistance >= distance) continue;

			//���܂łōł��߂��̂ŋL�^����
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		//���b�V���R���C�_�[�̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			XMVECTOR tempInter;

			//������Ȃ���Ώ��O
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter)) continue;
			//�������ŏ��łȂ���Ώ��O
			if (tempDistance >= distance) continue;

			//���܂łōł��߂��̂ŋL�^����
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}

	//�ŏI�I�ɉ����ɂ������Ă���Ό��ʂ���������
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

	//�S�ẴR���C�_�[�Ƒ�������`�F�b�N
	it = colliders3D_.begin();
	for (; it != colliders3D_.end(); ++it)
	{
		BaseCollider* col = *it;

		//����������Ȃ���΃X�L�b�v
		if (!(col->attribute_ & attribute))
		{
			continue;
		}

		// ��
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//�������ĂȂ�������
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject)) { continue; }

			// ���������Z�b�g
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// �N�G���[�R�[���o�b�N�Ăяo��
			if (!callBack->OnQueryHit(info)) {
				// �߂�l��false�̏ꍇ�A�p�������I��
				return;
			}
		}
		// ���b�V��
		else if (col->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			//�������ĂȂ�������
			if (!meshCollider->CheckCollisionSphere(sphere, &tempInter, &tempReject)) { continue; }

			// ���������Z�b�g
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// �N�G���[�R�[���o�b�N�Ăяo��
			if (!callBack->OnQueryHit(info)) {
				// �߂�l��false�̏ꍇ�A�p�������I��
				return;
			}
		}
	}
}

//-----------------------------------------------------
void CollisionManager::CheckAllCollisions()
{
	//3D�̂��ׂĂ̓����蔻��`�F�b�N
	CheckAllCollision3D();
	//2D�̂��ׂĂ̓����蔻��`�F�b�N
	CheckAllCollision2D();
}

//3D�̔���
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

			//�Ƃ��ɋ��̏ꍇ
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

			//���Ɩʂ̏ꍇ
			if ((typeA == COLLISIONSHAPE_PLANE || typeB == COLLISIONSHAPE_PLANE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				Plane* PlaneB = nullptr;
				DirectX::XMVECTOR inter;

				//A�����̎�
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_PLANE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					PlaneB = dynamic_cast<Plane*>(colB);
				}
				//A���O�p�`�̎�
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

			//���ƎO�p�`�̏ꍇ
			if ((typeA == COLLISIONSHAPE_TRIANGLE || typeB == COLLISIONSHAPE_TRIANGLE)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				Triangle* TriangleB = nullptr;
				DirectX::XMVECTOR inter;

				//A�����̎�
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_TRIANGLE)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					TriangleB = dynamic_cast<Triangle*>(colB);
				}
				//A���O�p�`�̎�
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

			//���b�V���Ƌ��̔���
			if ((typeA == COLLISIONSHAPE_MESH || typeB == COLLISIONSHAPE_MESH)
				&&
				(colA->GetIsValid() && colB->GetIsValid()))
			{
				Sphere* SphereA = nullptr;
				MeshCollider* meshCollider = nullptr;
				DirectX::XMVECTOR inter;

				//A�����̎�
				if (typeA == COLLISIONSHAPE_SPHERE && typeB == COLLISIONSHAPE_MESH)
				{
					SphereA = dynamic_cast<Sphere*>(colA);
					meshCollider = dynamic_cast<MeshCollider*>(colB);
				}
				//A�����b�V���R���C�_�[�̎�
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

	//2D�̂�
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

			//�Ƃ��ɉ~�̏ꍇ
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
