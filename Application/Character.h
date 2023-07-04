#pragma once
#include"Object.h"
#include"Weapon.h"


class Character : public Object
{
protected:
	//•Ší
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

protected:

public:
	//•Ší
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//•ŠíE‚¤
	void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//•Ší—‚Æ‚·A“Š‚°‚é
	void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);
};