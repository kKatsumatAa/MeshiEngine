#include "Character.h"

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	SetWeapon(weapon);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
	//所有者の判定属性を入れる
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
	//仮で手から離れたらアイテムの属性にする
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
