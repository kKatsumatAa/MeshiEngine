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
	const float SHOT_POS_EXTEND_ = 5.0f;
	//武器を持つノード
	const std::string WEAPON_PARENT_NODE_NAME_ = "RightHand";
	//武器の角度
	const Vec3 WEAPON_ROT_ = { PI / 2.0f,PI / 2.0f,0 };


private:
	const float PARTICLE_SIZE_EXTEND_ = 2.0f;
	//被ダメージ時のよろめき時間
	const float DAMAGED_MAX_TIME_ = 20.0f;
	//死亡時
	const float DEAD_TIMER_MAX_ = 60.0f;
	//分割数
	const float TESS_FACTOR_MAX_ = 20.0f;

private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.45f;
	bool isCantMove = false;

	//ウェーブ番号
	int32_t waveNum_ = -1;

	//出現時のクールタイム
	float emergeCoolTime_ = 0;

	//向きをプレイヤーに向けるためのクォータニオン
	Quaternion directionQua_;
	//回転の時間
	float directionRotTime_ = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	Vec3 directionVec_;

	float damageCoolTime_ = 0;
	float velocityLength_ = 0;

	const int8_t HP_TMP_ = 3;

	const float KNOCK_BACK_POW_ = 0.365f;

	const float DISSOLVE_POW_ = 0.3f;

	const float WEAPON_FALL_VEL_EXTEND_ = 1.5f;

	const float PLAYER_DISTANCE_EXTEND_ = 0.9f;

	//プレイヤーが視界にいるか
	bool isAttacking_ = false;

	//出現演出
	const int32_t LIGHT_INDEX_INIT_ = -1;
	int32_t lightIndexTmp_ = LIGHT_INDEX_INIT_;
	DirectX::XMFLOAT3 EMERGE_COL_ = { 1.0f,0,0 };

	//構えるときとかに徐々にアニメーションゆっくりにするため
	float animeSpeedExtend_ = 1.0f;

	//被攻撃時の加算角度
	std::vector<DamagedNodeAddRot> damagedAddRots_;

	//アニメーション中に地面にウェーブ出す
	float walkWaveTimer_ = 0;
	float beforeWalkTime_ = 0;
	const float  WALK_MOVE_INTERVAL_ = 20.f;

	//ゲーム的に死亡したか
	bool isDead_ = false;

	//ステート
	std::unique_ptr<EnemyState> state_ = nullptr;
	std::unique_ptr<EnemyState> stanceState_ = nullptr;

public:
	static const float S_LENGTH_MAX_;


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
	void KnockBack(const CollisionInfo& info);
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
};
