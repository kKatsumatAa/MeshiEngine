#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Weapon.h"


class Gun :
	public Weapon
{
private:
	//攻撃のクールタイム
	const float SHOT_COOL_TIME_MAX_ = 35;

	const float BULLET_VELOCITY_ = 2.3f;

	//弾の残量
	const int32_t BULLETS_TMP_ = 3;
	int32_t remainingBullets_ = BULLETS_TMP_;

	//発射位置
	Vec3 shotPos_ = { 0,0,0 };

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Gun> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

private:
	////所有者がいないときに飛んでいく
	//void NoParentMove() override;

	void ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize = 1.0f);
	void ParticleGenerate()override { ; }

	//地形と当たった時の処理
	void OnLandShape(const Vec3& interPos)override;


public:
	//射撃
	void Attack(const Vec3& directionVec, int32_t decreBullet = 1, Object* owner = nullptr, float particleSize = 1.0f) override;

	//持ち主変更
	void ChangeOwner(Object* parent) override;

	//
	int32_t GetBulletNum() { return remainingBullets_; }
	//クールタイムの終わりまでの割合(0~1.0f)
	float GetAttackCoolTimeRatio() override;

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model) override;

	void Update() override;

	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
};

