#pragma once
#include"Object.h"
#include"Weapon.h"
#include"CollisionManager.h"


class Character : public Object
{
protected:
	//����
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

	bool isOnGround = false;
	bool isOldOnGround = false;

protected:

public:
	//����
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//����E��
	void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//���헎�Ƃ��A������
	void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);

	//�n�ʂƂ̔���
	void GroundCollision();
};