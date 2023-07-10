#pragma once
#include"Object.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include<functional>


class Character : public Object
{
protected:
	//武器
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

	//地面と当たっているか
	bool isOnGround_ = false;

	//下向き加速度
	const float FALL_ACC_ = -0.03f;
	const float FALL_V_Y_MIN_ = -0.8f;
	Vec3 fallVec_;

	const float JUMP_V_Y_FIST_ = 0.7f;//ジャンプ時上向き初速

protected:

public:
	//武器
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//武器拾う
	void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//武器落とす、投げる
	void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);

	//地面との判定
	void GroundUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);
};