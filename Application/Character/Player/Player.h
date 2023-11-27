#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "PlayerHandManager.h"
#include "Gun.h"
#include "Character.h"


class PlayerAttackState;
class PlayerState;

class Player :
	public Character
{
public:
	const Vec3 WEAPON_POS_EXTEND_ = { -1.0f,-0.5f,-2.0f };

private:
	const float DEAD_TIMER_MAX_ = 70.0f;
	float const DEAD_MIN_POS_EXTEND_ = -4.0f;

private:
	const int8_t HP_TMP_ = 1;

	const float VELOCITY_TMP_ = 0.6f;
	//マウスでのカメラ回転減衰
	const float MOUSE_GAME_VEL_ATTEN_ = 0.7f;
	Vec2 mouseVel_ = { 0,0 };

	Vec3 cameraRot_ = { 0,0,0 };

	Vec3 frontVec_ = { 0,0,1.0f };
	Vec3 upVec_ = { 0,1.0f,0 };
	Vec3 rightVec_ = { 0,0,0 };

	//攻撃中か
	bool isAttacking_ = false;
	//攻撃可能か
	bool isTarget_ = false;

	//左クリックしたか
	bool isClickLeft_ = false;

	//攻撃が届く距離
	const float ATTACK_LENGTH_ = 11.0f;

	//マウスのカメラ移動スピード
	const float MOUSE_VELOCITY_TMP_ = 0.0005f;
	//移動によるゲームスピード加算の倍率
	const float MOVE_ADD_VEL_EXTEND_ = 0.052f;

	//手のマネージャークラス
	std::unique_ptr<PlayerHandManager> handManager_ = nullptr;

	//状態のステート
	std::unique_ptr<PlayerState> state_ = nullptr;

	bool isDead_ = false;

	//銃を撃った敵の座標
	Vec3 bulletOwnerEnemyPos_ = { 0,0,0 };

	const float FALL_VEL_POW_ = 2.0f;

public:
	~Player();

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Player> Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

private:
	//倒されたあとに呼び出す
	void Dead(const CollisionInfo& info);

private:
	//uiの位置更新
	void UpdateUI();

public:
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	void SetIsTarget(bool is) { isTarget_ = is; }

	bool GetIsAttacking() { return isAttacking_; }
	bool GetIsCanAttack() { return isTarget_; }

	float GetAttackLength() { return ATTACK_LENGTH_; }

	//手のマネージャー
	PlayerHandManager* GetHandManager() { return handManager_.get(); }

public:
	bool Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

	void Update() override;

	void Draw() override;

	void ChangePlayerState(std::unique_ptr<PlayerState> state);

	void OnCollision(const CollisionInfo& info) override;

public:
	void ThrowWeapon();
	//移動
	void Move();
	//向きを変える
	void DirectionUpdate();

public:
	void SetIsClickLeft(bool isClickLeft) { isClickLeft_ = isClickLeft; }
	bool GetIsClickLeft() { return isClickLeft_; }

	Vec3 GetWeaponPosTmp();

	const Vec3& GetBulletOwnerEnemyPos() { return bulletOwnerEnemyPos_; }
};

