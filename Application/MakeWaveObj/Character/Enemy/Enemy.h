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
	//ダメージ時のノードに加算する角度
	struct DamagedNodeAddRot
	{
		std::string nodeName;
		Vec3 addRotBegin = { 0,0,0 };
		Vec3 addRotEnd = { 0,0,0 };
	};

private:
	const float PARTICLE_SIZE_EXTEND_ = 2.0f;
	//被ダメージ時のよろめき時間
	const float DAMAGED_MAX_TIME_ = 20.0f;
	//死亡時
	const float DEAD_TIMER_MAX_ = 60.0f;
	//分割数
	const float TESS_FACTOR_MAX_ = 10.0f;

private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.45f;
	bool isCantMove = false;

	//ウェーブ番号
	int32_t waveNum_ = -1;

	//出現時のクールタイム
	float coolTime_ = 0;

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
	const float  WALK_MOVE_INTERVAL_ = 5.f;

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

private:
	//被弾時のパーティクル
	void DamageParticle(int32_t particleNum, uint64_t interval, float vecPow = 1.0f, const CollisionInfo* info = nullptr, IObject3D* obj = nullptr, Vec3* pos = nullptr,
		const Vec3& offsetPosExtend = { 0,0,0 });

public:
	//ノードごとにパーティクルだす
	void DeadNodesParticle(int32_t particleNum, uint64_t interval);

public:
	~Enemy();

public:
	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, float coolTime, Weapon* weapon, IModel* model);

	void EmergeInitialize();

	void Update() override;

	void DrawShadow()override;
	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;
	//当たり判定時の処理を行う
	void OnCollision(IObject3D* obj, const CollisionInfo& info);

	void ChangeEnemyState(std::unique_ptr<EnemyState> state);
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
	int32_t GetWaveNum() { return waveNum_; }
	float GetCoolTime() { return coolTime_; }
	void DecrementCoolTime();
	//ライト
	int32_t GetLightIndexTmp() { return lightIndexTmp_; }
	int32_t GetLightIndexInit() { return LIGHT_INDEX_INIT_; }
	//被ダメージ時の加算角度
	const std::vector<DamagedNodeAddRot>& GetDamagedAddRots() { return damagedAddRots_; }

public:
	//攻撃中か
	void SetIsAttacking(bool isAttacking) { isAttacking_ = isAttacking; }
	bool GetIsAttacking() { return isAttacking_; }
	//ゲーム的に死亡したか
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool GetIsDead() { return isDead_; }
	//構えるときとかに徐々にゆっくりにするための変数
	void SetAnimeSpeedExtend(float animeSpeedExtend) { animeSpeedExtend_ = animeSpeedExtend; }
	float GetAnimeSpeedExtend() { return animeSpeedExtend_; }
};
