#include "Character.h"

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	Vec3 localP = { GetScale().x_ ,-GetScale().y_ / 2.0f ,GetScale().z_ * 2.0f };
	if (localPos)
	{
		localP = *localPos;
	}

	SetWeapon(weapon);
	weapon_->SetLocalPos(localP);
	weapon_->ChangeOwner(GetWorldMat());
}

void Character::FallWeapon(const Vec3& directionVec, Vec3* localPos)
{
	if (localPos)
	{
		GetWeapon()->SetLocalPos(*localPos);
	}

	GetWeapon()->ChangeOwner(nullptr);
	GetWeapon()->SetFallVec(directionVec);
	SetWeapon(nullptr);
}
