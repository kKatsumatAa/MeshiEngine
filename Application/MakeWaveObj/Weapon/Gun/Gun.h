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
private:
	//攻撃のクールタイム
	const float SHOT_COOL_TIME_MAX_ = 70;

	const float BULLET_VELOCITY_ = 1.8f;

	//弾の残量
	const int32_t BULLETS_TMP_ = 3;
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
	//影用の深度描画
	void DrawShadow() override;
	//描画
	void Draw() override;

	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;
};

