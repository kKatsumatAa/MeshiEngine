#pragma once
#include"Object.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include<functional>
#include"ImGuiManager.h"


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
	const float FALL_ACC_ = -0.15f;
	const float FALL_V_Y_MIN_ = -3.5f;
	Vec3 fallVec_;

	const float JUMP_V_Y_FIST_ = 1.7f;//ジャンプ時上向き初速

	//壁と認識する角度
	static const float IS_WALL_ROT_;

	//ダメージ受けるかどうか(デバッグ用)
	bool isValidDamage_ = true;

protected:
	/// <summary>
	/// ダメージ受ける処理
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="deadFunc">死んだときに呼び出す処理</param>
	virtual void Damaged(int32_t damage, std::function<void()> deadFunc);

public:
	virtual ~Character() { ; }

public:
	virtual void Update();

	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	//武器
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//武器拾う
	virtual void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//武器落とす、投げる
	virtual void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);

	//地面と壁との判定
	virtual void OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);

	//クエリコールバック
	virtual void QueryCallBackUpdate();

	void SetIsValid(bool isValid)override;

	//みている方向に何かあったら
	bool CheckRayOfEyeHit(const Vec3& dir, float length, uint16_t attr, RaycastHit* info = nullptr);
};