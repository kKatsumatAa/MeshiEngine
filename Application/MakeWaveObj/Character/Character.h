/*
* @file Character.h
* @brief キャラクター全体の基底クラス（プレイヤーと敵の親クラス）
*/

#pragma once
#include"ObjectFBX.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include"Counter.h"
#include<functional>
#include "MakeWaveObj.h"


namespace BonePartName
{
	static const std::string HEAD = "Head";
	static const std::string SHOULDER = "Shoulder";
	static const std::string ARM = "Arm";
	static const std::string HAND = "Hand";
	static const std::string NECK = "Neck";
	static const std::string SPINE = "Spine";
	static const std::string SPINE_2 = "Spine2";
	static const std::string LEG = "Leg";
	static const std::string FOOT = "Foot";
	static const std::string TOE = "Toe";
}

namespace PartName
{
	static const std::string HEAD = "Head";
	static const std::string BODY = "Body";
	static const std::string RIGHT_HAND = "RightHand";
	static const std::string LEFT_HAND = "LeftHand";
	static const std::string RIGHT_LEG = "RightLeg";
	static const std::string LEFT_LEG = "LeftLeg";
}

class Character : public ObjectFBX, public IMakeWaveObj
{
public:
	const Vec3 WEAPON_FALL_VEC_ = { 0,-1.0f,0 };
	const Vec3 ON_GROUND_RAY_VEC_ = { 0,-1.0f,0 };
	const float ADS_DISTANCE_RATE_ = 0.2f;
	//死亡判定のhp
	const int8_t DEAD_HP_ = 0;
	//地面からの距離
	const float LENGTH_RATE_FROM_GROUND_ = 2.0f;
	//下向き加速度
	const float FALL_ACC_ = -0.15f;
	const float FALL_V_Y_MIN_ = -3.5f;
	const float JUMP_V_Y_FIST_ = 1.7f;//ジャンプ時上向き初速
	//壁と認識する角度
	static const float IS_WALL_ROT_;

protected:
	//武器
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;
	int8_t oldHP_ = 0;
	//殴ってる途中
	bool isBareAttack_ = false;
	//地面と当たっているか
	bool isOnGround_ = false;
	Vec3 fallVec_;
	//ダメージ受けるかどうか(デバッグ用)
	bool isValidDamage_ = true;
	//死亡演出のタイマー上限
	float deadEffectTimerMax_ = 0;
	//ゲーム上で死亡したか（演出用）
	bool isDead_ = false;
	//殴られたか
	bool isPunched_ = false;
	//カウンター
	Counter counter_;
	//攻撃してきた敵の位置
	Vec3 posOfEnemyAttack_ = { 0,0,0 };
	float shotPosExtend_ = 0;
	//体の長さ
	float bodyLength_ = 1.0f;
	//歩く速度
	float walkSpeedRate_ = 1.0f;

public:
	/// <summary>
	/// ダメージ受ける処理
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="deadFunc">死んだときに呼び出す処理</param>
	virtual void Damaged(int8_t damage, std::function<void()> deadFunc, std::function<void()> notDeadFunc = NULL);

public:
	//デストラクタ
	virtual ~Character();

public:
	//更新
	virtual void Update();
	//ImGuiの描画
	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	//武器のポインタ取得
	Weapon* GetWeapon() { return weapon_; }
	//武器のポインタセット
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//hp取得
	int8_t GetHP() { return hp_; }

	//死亡フラグ取得
	bool GetIsDead() { return isDead_; }
	//死亡フラグセット
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	//素手で攻撃中か取得
	bool GetIsBareAttack() { return isBareAttack_; }
	//素手で攻撃中かセット
	void SetIsBareAttack(bool isBareAttack) { isBareAttack_ = isBareAttack; }

	//殴られたかセット
	void SetIsPunched(bool isPunched) { isPunched_ = isPunched; }

	//攻撃してきた敵の位置をセット
	void SetPosOfEnemyAttack(const Vec3& pos) { posOfEnemyAttack_ = pos; }
	//攻撃してきた敵の位置を取得
	const Vec3& GetPosOfEnemyAttack() { return posOfEnemyAttack_; }

	//武器拾う
	virtual void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//武器落とす、投げる
	virtual void FallHaveWeapon(const Vec3& directionVec, Vec3* localPos = nullptr, bool isUpdatePos = true);
	virtual void FallWeapon(Weapon* weapon, const Vec3& directionVec, Vec3* localPos = nullptr, bool isUpdatePos = true);

	//地面と壁との判定
	virtual void OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);

	//クエリコールバック
	virtual void QueryCallBackUpdate();

	//更新等の有効フラグセット
	void SetIsValid(bool isValid)override;

	//みている方向に何かあったら
	bool CheckRayOfEyeHit(const Vec3& dir, float length, uint16_t attr, RaycastHit* info = nullptr);

public:
	//死亡演出のタイマー上限取得
	float GetDeadEffectTimerMax() { return deadEffectTimerMax_; }
	//カウント取得
	uint64_t GetCount() { return counter_.GetCount(); }

public:
	//倒されたあとに呼び出す
	virtual void Dead() = 0;
	//殴られた時の処理
	virtual void Punched(const CollisionInfo& info, IObject3D* nodeObj = nullptr) = 0;
public:
	//部位によってボーンごとの判定を無効に
	void InvalidPartNodeColliders(const std::string& partName);
protected:
	//攻撃を受けたボーンでそのボーンがある部位の名前取得
	std::string GetPartName(const std::string& boneName);
	//まだ部位のメッシュが残ってるか
	bool GetIsPartStillAttached(const std::string& partName);
public:
	//下半身あるか
	bool GetIsHavingBottomBody();
	//頭あるか
	bool GetIsHavingHead();
};