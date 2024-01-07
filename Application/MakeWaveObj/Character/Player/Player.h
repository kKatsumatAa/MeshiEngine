/*
* @file Player.h
* @brief プレイヤーの処理
*/

#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "PlayerHandManager.h"
#include "Gun.h"
#include "Character.h"


class PlayerHandState;
class PlayerReplayState;
class PlayerState;
class Replay;

class Player :
	public Character
{
public:
	const Vec3 WEAPON_POS_EXTEND_ = { -1.0f,-0.5f,-2.0f };
	//銃の弾の発射位置の倍率
	const float SHOT_POS_EXTEND_ = 1.5f;

	const Vec3 DEAD_WEAPON_FALL_VEC_ = { 0,-3.0f,0 };
	const float CAMERA_ROT_RIMIT_ = PI / 2.0f * 0.9f;
	const float EYE_RAY_LENGTH_ = 10000.0f;
	const float JUMPING_ADD_GAME_VEL_ = -0.001f;
public://ui系
	const float RETICLE_SIZE_MIN_ = 0.2f;
	const float RETICLE_SIZE_MAX_ = 1.5f;
	const float RETICLE_LENGTH_MAX_ = 50.0f;
	const float RAY_HIT_ENEMY_RETICLE_SIZE_MIN_ = 0.8f;
	const float RETICLE_DIST_SCALE_RATIO_MAX_ = 1.0f;
public://武器
	const Vec3 THROW_WEAPON_VEC_ = { 0,0.2f,0 };
	const int8_t DECREMENT_BULLET_NUM_ = 1;
	const float THROW_WEAPON_ADD_GAME_VEL_ = 0.9f;
private:
	const float DEAD_TIMER_MAX_ = 70.0f;
	float const DEAD_MIN_POS_EXTEND_ = -4.0f;

	const float SCALE_EXTEND_ = 1.35f;

	const float HEIGHT_FROM_GROUND_ = 4.5f;
	const int8_t HP_TMP_ = 1;

	const float VELOCITY_TMP_ = 0.6f;
	//マウスでのカメラ回転減衰
	const float MOUSE_GAME_VEL_ATTEN_ = 0.7f;
	//攻撃が届く距離
	const float ATTACK_LENGTH_ = 11.0f;

	//マウスのカメラ移動スピード
	const float MOUSE_VELOCITY_TMP_ = 0.0005f;
	//移動によるゲームスピード加算の倍率
	const float MOVE_ADD_VEL_EXTEND_ = 0.052f;
	const float FALL_VEL_POW_ = 2.0f;

private:
	//リプレイのポインタ
	std::unique_ptr<Replay> replay_ = nullptr;
	//リプレイのステートによって変わる処理の関数
	std::function<void()>replayStateF_ = NULL;

private:
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
	//右クリック
	bool isClickRight_ = false;


	//手のマネージャークラス
	std::unique_ptr<PlayerHandManager> handManager_ = nullptr;

	//リプレイ状態のステート
	std::unique_ptr<PlayerReplayState> replayState_ = nullptr;
	//素手か武器を持っているか等のステート
	std::unique_ptr<PlayerState>playerState_ = nullptr;



public:
	//デストラクタ
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
	void Dead()override;
	//溶岩で死んだか
	void UpdateLavaDead();
	//溶岩で死んだら
	void DeadLava();
	//殴られたら
	void Punched(const CollisionInfo& info, IObject3D* nodeObj = nullptr)override;

private:
	//uiの位置更新
	void UpdateUI();
	//マウスのクリックでプレイヤーのアクションをするかどうかをセット
	void UpdatePlayerActionFromMouse();

public:
	//セットされたリプレイのポインタ
	Replay* GetReplay() { return replay_.get(); }

public:
	//攻撃中かセット
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	//ターゲット中かセット
	void SetIsTarget(bool is) { isTarget_ = is; }

	//攻撃中か取得
	bool GetIsAttacking() { return isAttacking_; }
	//攻撃できるか取得
	bool GetIsCanAttack() { return isTarget_; }
	//攻撃の長さ取得
	float GetAttackLength() { return ATTACK_LENGTH_; }
	//カメラの角度取得
	const Vec3& GetCameraRot() { return cameraRot_; }

	//手のマネージャー取得
	PlayerHandManager* GetHandManager() { return handManager_.get(); }

	//リプレイのステートの初期化
	void InitializeReplayState();

public:
	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);
	//更新
	void Update() override;
	//描画
	void Draw() override;

public:
	//リプレイのステートを変更
	void ChangePlayerReplayState(std::unique_ptr<PlayerReplayState> state);
	//リプレイ再生状態に変更する
	void ChangeToReplayingState();
	//素手などのステートを変更
	void ChangePlayerState(std::unique_ptr<PlayerState> state);
	//プレイヤーステート取得
	PlayerState* GetPlayerState() { return playerState_.get(); }

	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;

public:
	//武器を投げる
	void ThrowWeapon();
	//移動
	void Move();
	//向きを変える
	void DirectionUpdate();
	//正面ベクトルを回転
	void RotateFrontVec(const Vec3& rot);
	//正面ベクトルを使ってカメラのターゲットを設定
	void UpdateUseCameraTarget();

public:
	//方向の更新の計算などの処理
	void DirectionUpdateCalcPart(const Vec2& mouseVec);
	//移動の計算などの処理
	void MoveCalcPart(bool leftKey, bool rightKey, bool upKey, bool downKey, bool spaceKey, float moveVelRatio = 1.0f);

public:
	//左クリックアクションするか取得
	bool GetIsClickLeft() { return isClickLeft_; }
	//左クリックアクションするかセット
	void SetIsClickLeft(bool isClickLeft) { isClickLeft_ = isClickLeft; }
	//右クリックアクションするか取得
	bool GetIsClickRight() { return isClickRight_; }
	//右クリックアクションするか取得
	void SetIsClickRight(bool isClickRight) { isClickRight_ = isClickRight; }

	//武器のローカル座標取得
	Vec3 GetWeaponPosTmp();

	//正面ベクトルから飛ばしたレイのターゲットまでのベクトル(正規化なし)
	Vec3 GetFrontTargetVec(uint16_t colAttr);

public:
	//リプレイの状態でする処理をセット
	void SetReplayStateF(std::function<void()>f) { replayStateF_ = f; }
};

