#pragma once
#include"ObjectFBX.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include"Counter.h"
#include<functional>
#include "MakeWaveObj.h"


class Character : public ObjectFBX, public IMakeWaveObj
{
protected:
	//武器
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;
	int8_t oldHP_ = 0;

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

	//死亡時のタイマー上限
	float deadTimerMax_ = 0;

	//ゲーム上で死亡したか（演出用）
	bool isDead_ = false;

	//カウンター
	Counter counter_;

	//攻撃してきた敵の位置
	Vec3 posOfEnemyAttack_ = { 0,0,0 };

public:
	/// <summary>
	/// ダメージ受ける処理
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="deadFunc">死んだときに呼び出す処理</param>
	virtual void Damaged(int8_t damage, std::function<void()> deadFunc, std::function<void()> notDeadFunc = NULL);

public:
	virtual ~Character() { ; }

public:
	virtual void Update();

	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	//武器
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//hp
	int8_t GetHP() { return hp_; }

	//死亡フラグ
	bool GetIsDead() { return isDead_; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	//攻撃してきた敵の位置を保存
	void SetPosOfEnemyAttack(const Vec3& pos) { posOfEnemyAttack_ = pos; }
	const Vec3& GetPosOfEnemyAttack() { return posOfEnemyAttack_; }

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

public:
	float GetDeadTimerMax() { return deadTimerMax_; }

	uint64_t GetCount() { return counter_.GetCount(); }

public:
	//倒されたあとに呼び出す
	virtual void Dead() = 0;
};