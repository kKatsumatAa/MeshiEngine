/*
* @file Enemy.h
* @brief 敵（fbx）
*/

#pragma once
#include "ObjectFBX.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"
#include "Character.h"


class EnemyState;

class Enemy :
	public Character
{
public:
	//アニメーションの番号
	enum AnimationNum
	{
		WALK,
		PUNCH
	};

public:
	//ダメージ時のノードに加算する角度
	struct DamagedNodeAddRot
	{
		std::string nodeName;
		Vec3 addRotBegin = { 0,0,0 };
		Vec3 addRotEnd = { 0,0,0 };
	};

public:
	static const float S_LENGTH_MAX_;
public:
	//武器
	const Vec3 WEAPON_ROT_ = { PI / 2.0f,-PI / 2.0f,0 };
	const float SHOT_POS_EXTEND_ = 5.0f;
	const float THROW_WEAPON_VEC_Y_ = 0.4f;
	const float WEAPON_FALL_VEL_EXTEND_ = 1.5f;
	const std::string WEAPON_PARENT_NODE_NAME_ = PartName::LEFT_HAND;
	//消費する弾
	const int8_t CONSUM_BULLET_NUM_ = 0;
	//ライト
	const XMFLOAT3 POINT_LIGHT_ATTEN_ = { 0.977f,0.493f,0.458f };
	const XMFLOAT3 POINT_LIGHT_COLOR_ = { 90.0f,-1.0f,-1.0f };
	const XMFLOAT3 POINT_LIGHT_END_COLOR_ = { 0,0,0 };
	const int32_t LIGHT_INDEX_INIT_ = -1;
	//倍率
	const float INIT_DISSOLVE_RATE_ = 1.0f;
	const float WEAPON_SCALE_RATE_ = 2.0f;
	const Vec3 MODEL_MATERIAL_RATE_ = { 0.03f,0.1f,50.0f };
	const float VELOCITY_RATE_ = 0.45f / 1.5f;
	const float RATE_MAX_ = 1.0f;
	//色
	const Vec4 COLOR_ = { 3.0f,0,0,1.0f };
	//初期値
	const Vec3 INIT_VELOCITY_ = { 0,0,0 };
	const float INIT_DAMAGE_COOL_TIME_ = 0;
	//殴られたときなどに上に移動しないように
	const float DIR_Y_TMP_ = 0;
	const float DAMAGE_COOL_TIME_TMP_ = 20.0f;
	//ダメージのクールタイムが終わる速度
	const float END_VEL_OF_DAMAGE_COOL_ = 0;
	//ダメージのクールタイムが終わる時間
	const float DAMAGE_COOL_TIME_END_ = 0;
	//回転の時に使う軸
	const Vec3 USE_QUATERNION_AXIS_ = { 0,1.0f,0 };
	//アニメーション
	const float ANIMATION_SPEED_RATE_ = 3.0f;
	//押し戻し
	const float PUSH_BACK_VEL_RATE_ = 0.63f;
	const float PUSH_BACK_LENGTH_RATE_ = 1.001f;
	//地面に足がつく間隔
	const float  WALK_MOVE_INTERVAL_ = 20.0f;
	//HP
	const int8_t HP_TMP_ = 3;
	//何かしら衝撃を受けたとき
	const float KNOCK_BACK_POW_ = 0.365f;
	const float OBJ_DIST_VEC_Y_ = 0.0f;
	//ディゾルブ
	const float DISSOLVE_POW_ = 0.3f;
	//プレイヤーとの距離
	const float PLAYER_DISTANCE_EXTEND_ = 0.9f;
public:
	//パーティクル
	const int16_t PARTICLE_LIFE_TIME_ = 20;
	const float PARTICLE_SCALE_RATE_ = 1.0f / 30.0f;
	const float PARTICLE_VEL_RATE_ = 1.0f / 16.0f;
	const Vec3 PARTICLE_ACCEL_ = { 0,-0.0002f,0 };
	const XMFLOAT4 PARTICLE_START_COLOR_ = { 3.0f,0.02f,0.02f,0.95f };
	const XMFLOAT4 PARTICLE_END_COLOR_ = { 3.0f,0.02f,0.02f,0.95f };
	const float PARTICLE_START_ROT_ = PI * 4.0f;
	const float PARTICLE_END_ROT_ = -PI * 4.0f;
	const int8_t PUNCHED_PARTICLE_NUM_ = 60;
	const int8_t PUNCHED_PARTICLE_INTERVAL_ = 1;
	const float PUNCHED_PARTICLE_VEC_POW_ = 1.0f;
	const float PARTICLE_VEL_RATE_MIN_ = -0.1f;
	const float PARTICLE_VEL_RATE_MAX_ = 1.0f;
	//種類別
	const int8_t SHOOTED_PARTICLE_NUM_ = 100;
	const float SHOOTED_PARTICLE_VEL_RATE_ = 1.3f;
	const float DEAD_PARTICLE_VEL_RATE_ = 0.35f;
public://ノード
	const float NODE_COLLIDER_SCALE_ = 14.0f;
	const Vec3 INIT_NODE_ADD_ROT_ = { 0,0,0 };
	const float NODE_ADD_ROT_RATE_ = 1.0f / 6.0f;
	const float LEG_ADD_ROT_RATE_ = 1.0f / 5.5f;
	//種類別
	const float PUNCHED_NODE_ADD_ROT_RATE_ = 1.0f;
	const float SHOOTED_NODE_ADD_ROT_RATE_ = 1.5f;
	const float HIT_WEAPON_NODE_ADD_ROT_RATE_ = 0.7f;
public:
	//メッシュのオフセット
	const float MESH_OFFSET_INTERVAL_MIN_ = 95.0f;
	const float MESH_OFFSET_INTERVAL_MAX_ = 125.0f;
	const float MESH_OFFSET_LENGTH_RATE_ = 0.007f;
public://波紋
	//ステージの波紋
	const float STAGE_WAVE_WIDTH_RATE_ = 0.5f;
	const float STAGE_WAVE_HEIGHT_RATE_ = 0.2f;
	const float STAGE_WAVE_DISTANCE_RATE_ = 20.0f;
	const float STAGE_WAVE_TIME_ = 130.0f;
	//ダメージを受けたときのステージの波紋
	const float DAMAGE_STAGE_WAVE_WIDTH_RATE_ = 1.0f;
	const float DAMAGE_STAGE_WAVE_HEIGHT_RATE_ = 1.0f;
	const float DAMAGE_STAGE_WAVE_DISTANCE_RATE_ = 12.0f;
	const float DAMAGE_STAGE_WAVE_TIME_ = 22.0f;
	const float DAMAGE_STAGE_WAVE_TIME_RATE_MIN_ = 0.5f;
	const float DAMAGE_STAGE_WAVE_TIME_RATE_MAX_ = 2.0f;
	//ダメージを受けたときの自分の波紋
	const float DAMAGE_MY_WAVE_WIDTH_RATE_ = 1.0f / 20.0f;
	const float DAMAGE_MY_WAVE_HEIGHT_RATE_ = 1.0f / 18.0f;
	const float DAMAGE_MY_WAVE_DISTANCE_RATE_ = 2.0f;
	const float DAMAGE_MY_WAVE_TIME_ = 45.0f;
	const float DAMAGE_MY_WAVE_TIME_RATE_MIN_ = 1.0f;
	const float DAMAGE_MY_WAVE_TIME_RATE_MAX_ = 2.0f;
	//種類別倍率
	const float PUNCHED_WAVE_RATE_ = 1.4f;
	const float SHOOTED_WAVE_RATE_ = 0.65f;
	const float HIT_WEAPON_WAVE_RATE_ = 0.35f;
	//波紋数
	const int8_t STAGE_WAVE_NUM_ = 2;
	const int8_t MY_WAVE_NUM_ = 3;
private:
	const float EMERGE_COOL_TIME_MIN_ = 0.0f;
	const float PARTICLE_SIZE_EXTEND_ = 2.0f;
	//被ダメージ時のよろめき時間
	const float DAMAGED_MAX_TIME_ = 20.0f;
	//死亡時
	const float DEAD_TIMER_MAX_ = 60.0f;
	//分割数
	const float TESS_FACTOR_MAX_ = 20.0f;
public:
	const float DEAD_BODY_PART_VEC_MIN_ = -0.35f;
	const float DEAD_BODY_PART_VEC_MAX_ = 0.35f;

private:
	Vec3 velocity_ = { 0,0,0 };
	bool isCantMove = false;
	//ウェーブ番号
	int32_t waveNum_ = -1;
	//出現時のクールタイム
	float emergeCoolTime_ = 0;
	//向きをプレイヤーに向けるためのクォータニオン
	Quaternion directionQua_;
	Vec3 directionVec_;
	//ダメージクールタイム
	float damageCoolTime_ = 0;
	float velocityLength_ = 0;
	//プレイヤーが視界にいるか
	bool isAttacking_ = false;
	//出現演出
	int32_t lightIndexTmp_ = LIGHT_INDEX_INIT_;
	DirectX::XMFLOAT3 EMERGE_COL_ = { 1.0f,0,0 };
	//構えるときとかに徐々にアニメーションゆっくりにするため
	float animeSpeedExtend_ = 1.0f;
	//被攻撃時の加算角度
	std::vector<DamagedNodeAddRot> damagedAddRots_;
	//アニメーション中に地面にウェーブ出す
	float walkWaveTimer_ = 0;
	float beforeWalkTime_ = 0;
	//ゲーム的に死亡したか
	bool isDead_ = false;

	//ステート
	std::unique_ptr<EnemyState> state_ = nullptr;
	std::unique_ptr<EnemyState> stanceState_ = nullptr;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, float coolTime, Weapon* weapon,
		IModel* model);

private:
	//銃をノックバックして落とす
	void KnockBack(const CollisionInfo& info, bool IsfallingWeapon);
	//ターゲットに向かって動く処理のみ
	void WalkToTarget(const Vec3& targetPos, bool isWave = true);
	//攻撃された部位ごとにノードに加算する角度を決める
	void SetAllNodeAddRots(const IObject& nodeObj, float angleExtend = 1.0f);

	//歩くときに地面に波紋
	void WalkWaveUpdate();

	//倒されたあとに呼び出す
	void Dead()override;

private:
	//被弾時のパーティクル
	void DamageParticle(int32_t particleNum, uint64_t interval, float vecPow = 1.0f, const CollisionInfo* info = nullptr, IObject3D* obj = nullptr, Vec3* pos = nullptr,
		const Vec3& offsetPosExtend = { 0,0,0 });

public:
	//ノードごとにパーティクルだす
	void DeadNodesParticle(int32_t particleNum, uint64_t interval);

public:
	//デストラクタ
	~Enemy();

public:
	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, float coolTime, Weapon* weapon, IModel* model);
	//出現時の初期化処理
	void EmergeInitialize();
	//更新
	void Update() override;
	//影用の深度描画
	void DrawShadow()override;
	//描画
	void Draw()override;
	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;
	//当たり判定時の処理を行う
	void OnCollision(IObject3D* obj, const CollisionInfo& info);
	//状態変更
	void ChangeEnemyState(std::unique_ptr<EnemyState> state);
	//構えの状態変更
	void ChangeEnemyStanceState(std::unique_ptr<EnemyState> state);

public:
	//壁や地面のみと判定
	void CollisionWallAndFloor();
	//移動
	void AllMove(const Vec3& targetPos, bool isWave = true);
	//攻撃
	void Attack(const Vec3& targetPos);
	//向きを変更
	void DirectionUpdate(const Vec3& targetPos);
	//hp処理
	void HPUpdate(float t = 1.0f);
	//被ダメージ時に波発生
	void BeginDamagedWave(const CollisionInfo& info, float wavePow);

public:
	//プレイヤーが一定範囲内にいるか
	bool GetPlayerIsWithinRange();
	//殴られたら
	void Punched(const CollisionInfo& info, IObject3D* nodeObj = nullptr)override;

public:
	//出現するウェーブ番号取得
	int32_t GetWaveNum() { return waveNum_; }
	//出現のクールタイム取得
	float GetEmergeCoolTime() { return emergeCoolTime_; }
	//出現のクールタイム減らす
	void DecrementEmergeCoolTime();
	//ライトのインデックス取得
	int32_t GetLightIndexTmp() { return lightIndexTmp_; }
	//ライトのインデックスの初期値を取得（初期値は使わせないように）
	int32_t GetLightIndexInit() { return LIGHT_INDEX_INIT_; }
	//被ダメージ時の加算角度
	const std::vector<DamagedNodeAddRot>& GetDamagedAddRots() { return damagedAddRots_; }

public:
	//出現時のライトを非アクティブに
	void InactiveEmergeLight();
	//出現時の使用ライトの色
	void SetEmergeLight(float rate);

public:
	//攻撃中かをセット
	void SetIsAttacking(bool isAttacking) { isAttacking_ = isAttacking; }
	//攻撃中かを取得
	bool GetIsAttacking() { return isAttacking_; }
	//ゲーム的に死亡したかをセット
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	//ゲーム的に死亡したかを取得
	bool GetIsDead() { return isDead_; }
	//構えるときとかに徐々にゆっくりにするための変数セット
	void SetAnimeSpeedExtend(float animeSpeedExtend) { animeSpeedExtend_ = animeSpeedExtend; }
	//構えるときとかに徐々にゆっくりにするための変数取得
	float GetAnimeSpeedExtend() { return animeSpeedExtend_; }

private:
	//部位を分離する
	void DetachPart(const std::string& partName, const Vec3& throwDir, Vec3* partGeneratePos = nullptr);
	//部位を全て分離
	void DetachAllPart();
	//攻撃を受けたボーンでそのボーンがある部位の名前取得
	std::string GetPartName(const std::string& boneName);
	//武器持っている部位か
	bool GetNodeIsHavingWeaponPart(const std::string& nodeName);
};
