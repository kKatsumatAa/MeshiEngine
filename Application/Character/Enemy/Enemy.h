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

private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.6f;
	bool isCantMove = false;

	//ウェーブ番号
	int32_t waveNum_ = -1;

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

	const float DISSOLVE_POW_ = 0.5f;

	const float WEAPON_FALL_VEL_EXTEND_ = 1.2f;

	//出現演出
	const int32_t LIGHT_INDEX_INIT_ = -1;
	int32_t lightIndexTmp_ = LIGHT_INDEX_INIT_;
	DirectX::XMFLOAT3 EMERGE_COL_ = { 1.0f,0,0 };

	//被攻撃時の加算角度
	std::vector<DamagedNodeAddRot> damagedAddRots_;

	//ステート
	std::unique_ptr<EnemyState> state_ = nullptr;

public:
	static const float S_LENGTH_MAX_;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon,
		IModel* model);

private:
	//銃をノックバックして落とす
	void KnockBack(const CollisionInfo& info);
	//被弾時のパーティクル
	void DamageParticle(const CollisionInfo& info, IObject3D* obj = nullptr, const Vec3& offsetPosExtend = { 1.0f / 8.0f,1.0f / 8.0f,1.0f / 8.0f },
		int32_t particleNum = 60);
	//ターゲットに向かって動く処理のみ
	void WalkToTarget(const Vec3& targetPos);
	//攻撃された部位ごとにノードに加算する角度を決める
	void SetAllNodeAddRots(const IObject& nodeObj);

public:
	~Enemy();

public:
	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon, IModel* model);

	void EmergeInitialize();

	void Update() override;

	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;
	//当たり判定時の処理を行う
	void OnCollision(IObject3D* obj, const CollisionInfo& info);

	void ChangeEnemyState(std::unique_ptr<EnemyState> state);

public:
	//壁や地面のみと判定
	void CollisionWallAndFloor();
	//移動
	void AllMove(const Vec3& targetPos);
	//攻撃
	void Attack(const Vec3& targetPos);
	//向きを変更
	void DirectionUpdate(const Vec3& targetPos);
	//hp処理
	void HPUpdate(float t = 1.0f);

public:
	int32_t GetWaveNum() { return waveNum_; }
	//ライト
	int32_t GetLightIndexTmp() { return lightIndexTmp_; }
	int32_t GetLightIndexInit() { return LIGHT_INDEX_INIT_; }
	//被ダメージ時の加算角度
	const std::vector<DamagedNodeAddRot>& GetDamagedAddRots() { return damagedAddRots_; }
};
