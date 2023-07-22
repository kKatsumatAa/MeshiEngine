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
	const float FALL_ACC_ = -0.15f;
	const float FALL_V_Y_MIN_ = -3.5f;
	Vec3 fallVec_;

	const float JUMP_V_Y_FIST_ = 1.7f;//�W�����v�����������

	//�ǂƔF������p�x
	static const float IS_WALL_ROT_;

protected:


public:
	~Character() { ; }

public:
	//����
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//����E��
	void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//���헎�Ƃ��A������
	void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);

	//�n�ʂƕǂƂ̔���
	void OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);

	//�N�G���R�[���o�b�N
	void QueryCallBackUpdate();
};