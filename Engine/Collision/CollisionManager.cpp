#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"


CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager sInstance;
	return &sInstance;
}

bool CollisionManager::Raycast(const Ray& ray, std::string notTargetName, RaycastHit* hitInfo,float maxDistance)
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

	// �S�ẴR���C�_�[�Ƒ�����`�F�b�N
	it = colliders_.begin();
	for (; it != colliders_.end(); ++it) {
		BaseCollider* colA = *it;
		//���̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE && colA->GetIsValid() && !colA->GetIs2D() && colA->GetObject3d()->GetObjName() != notTargetName) {
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
		if (colA->GetShapeType() == COLLISIONSHAPE_PLANE && colA->GetIsValid() && !colA->GetIs2D() && colA->GetObject3d()->GetObjName() != notTargetName) {
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
		if (colA->GetShapeType() == COLLISIONSHAPE_TRIANGLE && colA->GetIsValid() && !colA->GetIs2D() && colA->GetObject3d()->GetObjName() != notTargetName) {
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

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//�S�Ă̑g�ݍ��킹�ɂ��đ�������`�F�b�N
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

			//�Ƃ��ɋ��̏ꍇ
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

			//���Ɩʂ̏ꍇ
			if ((typeA == COLLISIONSHAPE_PLANE || typeB == COLLISIONSHAPE_PLANE)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Plane* PlaneB = dynamic_cast<Plane*>(colB);
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
				if (Collision::CheckSphere2Plane(*SphereA, *PlaneB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}

			//���ƎO�p�`�̏ꍇ
			if ((typeA == COLLISIONSHAPE_TRIANGLE || typeB == COLLISIONSHAPE_TRIANGLE)
				&&
				(colA->GetIsValid() && colB->GetIsValid())
				&&
				(!colA->GetIs2D() && !colB->GetIs2D()))
			{
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Triangle* TriangleB = dynamic_cast<Triangle*>(colB);
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
				if (Collision::CheckSphere2Triangle(*SphereA, *TriangleB, &inter))
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
