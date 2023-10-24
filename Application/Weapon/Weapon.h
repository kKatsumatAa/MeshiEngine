#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Weapon :
	public Object
{
protected:
	//フレームごとの倍率
	const float FRAME_VEL_EXTEND_ = 0.97f;

protected:
	//手を離れたときのベクトル
	Vec3 fallVec_ = { 0,0,0 };
	const float FALL_VEC_Y_MIN_ = -3.1f;
	//ローカル座標
	Vec3 localPos_ = { 0,0,0 };
	//持ち主
	IObject3D* owner_ = nullptr;

	//前回の位置
	Vec3 oldPos_ = { 0,0,0 };

	//投げられているか
	bool isThrowing_ = false;

	//クールタイム
	float attackCoolTime_ = 0;

	//重力
	const float GRAVITY_TMP_ = 0.15f;
	const float GRAVITY_MAX_ = 3.0f;

public:
	Weapon() { ; }
	virtual ~Weapon() { ; }

protected:
	//地形と当たった時に呼び出す
	virtual void OnLandShape(const Vec3& interPos) = 0;

	//所有者がいないときに飛んでいく
	virtual void NoParentMove();

	virtual void ParticleGenerate() = 0;

public:
	//攻撃
	virtual void Attack(const Vec3& directionVec, int32_t decreBullet = 1, IObject3D* owner = nullptr, float particleSize = 1.0f) = 0;

	//持ち主変更
	virtual void ChangeOwner(IObject3D* parent);

	//判定属性
	void SetAttribute(uint16_t attribute) { GetCollider()->SetAttribute(attribute); }

public:
	//
	virtual void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//ローカル座標セット
	virtual  void SetLocalPos(const Vec3& pos) {
		localPos_ = pos;
	}

	Vec3 GetLocalPos() { return localPos_; }

	//投げる、被弾して落ちるときのスピード
	virtual const Vec3& GetFallVelocity() { return fallVec_; }

	bool GetIsThrowing() { return isThrowing_; }
	void SetIsThrowing(bool isThrowing) { isThrowing_ = isThrowing; }

	//クールタイムの終わりまでの割合(0~1.0f)
	virtual float GetAttackCoolTimeRatio() = 0;

public:

	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model) override = 0;

	virtual void Update();

	virtual void OnCollision(const CollisionInfo& info) override = 0;

public:
	//古い座標の更新
	void OldPosUpdate() { oldPos_ = GetWorldTrans(); }
};

