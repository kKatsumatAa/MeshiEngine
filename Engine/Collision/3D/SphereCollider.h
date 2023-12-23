/*
* @file SphereCollider.h
* @brief 球のコライダー
*/

#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

/// <summary>
/// 球衝突判定オブジェクト
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;

private:
	// オブジェクト中心からのオフセット
	XMVECTOR offset_;
	// 半径
	float radius_;

public:
	//コンストラクタ
	SphereCollider(const XMVECTOR& offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset_(offset), radius_(radius)
	{
		// 球形状をセット
		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//半径セット
	inline void SetRadius(float radius) { radius_ = radius; }
	//半径取得
	inline float GetRadius() { return radius_; }
};

