#pragma once
#include"Object.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include<functional>


class Character : public Object
{
protected:
	//����
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

	//�n�ʂƓ������Ă��邩
	bool isOnGround_ = false;

	//�����������x
	const float FALL_ACC_ = -0.03f;
	const float FALL_V_Y_MIN_ = -0.8f;
	Vec3 fallVec_;

	const float JUMP_V_Y_FIST_ = 0.7f;//�W�����v�����������

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
	void GroundUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);
};