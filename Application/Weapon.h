#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Weapon :
	public Object
{
protected:
	//手を離れたときのベクトル
	Vec3 fallVec_ = { 0,0,0 };
	//ローカル座標
	Vec3 localPos_ = { 0,0,0 };


public:
	/*/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	virtual std::unique_ptr<Weapon> Create(std::unique_ptr<WorldMat> worldMat) = 0;*/

protected:
	//所有者がいないときに飛んでいく
	virtual void NoParentMove();

	virtual void ParticleGenerate() = 0;

public:
	//射撃
	virtual void Attack(const Vec3& directionVec, int32_t decreBullet = 1) = 0;

	//持ち主変更
	virtual void ChangeOwner(WorldMat* parent) = 0;

public:
	//
	virtual void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//ローカル座標セット
	virtual  void SetLocalPos(const Vec3& pos) {
		localPos_ = pos;
	}

	//投げる、被弾して落ちるときのスピード
	virtual const Vec3& GetFallVelocity() { return fallVec_; }

public:

	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat) override = 0;

	virtual void Update() override = 0;

	virtual void OnCollision(const CollisionInfo& info) override = 0;
};

