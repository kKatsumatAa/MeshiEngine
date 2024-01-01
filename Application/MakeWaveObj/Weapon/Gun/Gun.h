/*
* @file Gun.h
* @brief 銃の処理
*/

#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Weapon.h"
#include "ParticleManager.h"
#include "MakeWaveObj.h"


class Gun :
	public Weapon
{
public:
	const Vec3 MATERIAL_EXTEND_ = { 1.0f,100.0f,5000.0f };
	const float TIME_RATIO_MAX_ = 1.0f;
public:
	const float BULLET_SCALE_RATE_ = 0.4f;
	const int16_t BULLET_LIFE_TIME_ = 300;
	//攻撃のクールタイム
	const float SHOT_COOL_TIME_MAX_ = 70;
	const float BULLET_VELOCITY_ = 1.8f;
	//弾の残量
	const int32_t BULLETS_TMP_ = 3;
public:
	const XMFLOAT4 SHOOT_PARTICLE_START_COLOR_ = { 4.0f,4.0f,4.0f,1.1f };
	const XMFLOAT4 SHOOT_PARTICLE_END_COLOR_ = { 4.0f,4.0f,4.0f,0 };
public://波紋
	const float SHOOT_STAGE_WAVE_THICK_EXTEND_RATE_1_ = 9.0f;
	const float SHOOT_STAGE_WAVE_THICK_EXTEND_RATE_2_ = 7.5f;
	const float SHOOT_STAGE_WAVE_DISTANCE_RATE_ = 300.0f;
	const float SHOOT_STAGE_WAVE_TIME_1_ = 40.0f;
	const float SHOOT_STAGE_WAVE_TIME_2_ = 47.0f;
	//ステージに当たった時の
	const int8_t ON_STAGE_WAVE_NUM_ = 3;
	const int8_t ON_STAGE_WAVE_THICK_EXTEND_RATE_ = 3;
	const float ON_STAGE_WAVE_DISTANCE_RATE_MIN_ = 3.0f;
	const float ON_STAGE_WAVE_DISTANCE_RATE_MAX_ = 6.0f;
	const float ON_STAGE_WAVE_TIME_ = 30.0f;
public:
	const float PARTICLE_VEL_MIN_ = -0.5f;
	const float PARTICLE_VEL_MAX_ = 0.5f;
	const int8_t PARTICLE_NUM_ = 50;
	const int8_t PARTICLE_LIFE_TIME_ = 10;
	const float PARTICLE_VEL_RATE_ = 0.5f;
	//ステージに当たった時
	const int8_t ON_STAGE_PARTICLE_NUM_ = 20;
	const float ON_STAGE_PARTICLE_VEL_MIN_ = -0.2f;
	const float ON_STAGE_PARTICLE_VEL_MAX_ = 0.2f;
	const float ON_STAGE_PARTICLE_SCALE_RATE_MIN_ = 0.5f;
	const float ON_STAGE_PARTICLE_SCALE_RATE_MAX_ = 4.0f;
	const int8_t ON_STAGE_PARTICLE_LIFE_TIME_ = 30;
	const XMFLOAT4 ON_STAGE_PARTICLE_START_COLOR_ = { 0.001f,0.001f,0.01f,1.5f };
	const XMFLOAT4 ON_STAGE_PARTICLE_END_COLOR_ = { 0.1f,0.1f,0.1f,1.0f };

private:
	int32_t remainingBullets_ = BULLETS_TMP_;

	//発射位置
	Vec3 shotPos_ = { 0,0,0 };

	//発射位置の倍率
	float shotPosExtend_ = 5.0f;

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Gun> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

private:
	//パーティクル生成
	void ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize = 1.0f
		, ParticleManager::BLEND_NUM blendNum = ParticleManager::BLEND_NUM::TRIANGLE) override;

	//地形と当たった時の処理
	void OnLandShape(const Vec3& interPos)override;

public:
	//射撃
	void Attack(const Vec3& directionVec, int32_t decreBullet = 1, IObject3D* owner = nullptr, float particleSize = 1.0f) override;

	//持ち主変更
	void ChangeOwner(IObject3D* parent) override;

public:
	//弾数
	int32_t GetBulletNum() { return remainingBullets_; }
	//クールタイムの終わりまでの割合(0~1.0f)
	float GetAttackCoolTimeRatio() override;

	//発射位置の倍率取得
	void SetShotPosExtend(float extend) { shotPosExtend_ = extend; }

public:
	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model) override;
	//更新
	void Update() override;

	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;
};

