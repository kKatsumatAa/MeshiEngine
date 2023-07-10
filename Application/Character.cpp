#include "Character.h"

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	SetWeapon(weapon);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
	//���L�҂̔��葮��������
	weapon_->SetAttribute(GetCollider()->GetAttribute());
}

void Character::FallWeapon(const Vec3& directionVec, Vec3* localPos)
{
	if (localPos)
	{
		GetWeapon()->SetLocalPos(*localPos);
	}

	GetWeapon()->ChangeOwner(nullptr);
	GetWeapon()->SetFallVec(directionVec);
	//���Ŏ肩�痣�ꂽ��A�C�e���̑����ɂ���
	weapon_->SetAttribute(COLLISION_ATTR_ITEMS);
	SetWeapon(nullptr);
}

//void Character::GroundCollision()
//{
//	SetVelocity({ GetVelocity().x_,max(GetVelocity().y_ + 0.001f,-0.5f), GetVelocity().z_ });
//
//	Ray ray;
//	ray.start = { GetWorldTrans().x_,GetWorldTrans().y_,GetWorldTrans().z_ };
//	ray.dir = { 0,-1.0f,0 };
//	RaycastHit info;
//	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, "player", &info);
//
//	if (isRayHit && info.object->GetObjName() == "stage")
//	{
//
//	}
//
//	if (isOnGround)
//	{
//
//	}
//}
