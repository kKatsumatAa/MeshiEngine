#pragma once
#include"Object.h"
#include"Weapon.h"


class Character : public Object
{
protected:
	//����
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

protected:

public:
	//����
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//����E��
	void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//���헎�Ƃ��A������
	void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);
};