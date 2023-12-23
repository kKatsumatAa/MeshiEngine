/*
* @file Bullet.h
* @brief 銃の弾丸
*/

#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "MakeWaveObj.h"


class Bullet :
	public Object, public IMakeWaveObj
{
private:
	//生存時間
	float lifeTime_ = 300;

	//方向ベクトル
	Vec3 directionVec_ = { 0,0,0 };

	//所有者
	IObject* owner_ = nullptr;

	Vec3 ownerPos_ = { 0,0,0 };

	//前のフレームの位置
	Vec3 oldPos_ = { 0,0,0 };

	//弾道
	Object ballisticsObj_;
	float ballisticsLength = 0;
	const float BALLISTICS_LENGTH_MAX_ = 10;


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Bullet> Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);

private:
	//死亡処理（引数に判定属性）
	void Dead(const Vec3& interPos, uint64_t attr);
	//死亡処理
	void Dead(const CollisionInfo& info);

	//弾道用のオブジェクトの位置など
	void BallisticsUpdate();

public:
	//射撃主のオブジェクトのポインタ取得
	IObject* GetOwner() { return owner_; }
	//射撃主のオブジェクトの位置取得
	const Vec3& GetOwnerPos() { return ownerPos_; }

public:
	//初期化
	bool Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);
	//更新
	void Update() override;
	//影用の深度値描画
	void DrawShadow() override;
	//描画
	void Draw() override;
	
	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;
};

