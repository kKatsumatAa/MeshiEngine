/*
* @file CircleCollider.h
* @brief 円のコライダー
*/

#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 円衝突判定オブジェクト
/// </summary>
class CircleCollider : public BaseCollider, public Circle
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;

public:
	const float SCALE_RATE_ = 0.5f;
	//平均を求めるために使ってる数
	static const uint8_t S_MEAN_VALUE_NUM_ = 2;

public:
	//コンストラクタ
	CircleCollider(const Vec2 center = { 0,0 }, float radius = 1.0f)
	{
		Circle::center = center;
		Circle::iRadius = radius;

		// 球形状をセット
		shapeType_ = COLLISIONSHAPE_CIRCLE;

		is2D_ = true;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//半径セット
	inline void SetRadius(float radius) { Circle::iRadius = radius; }
	//半径取得
	inline float GetRadius() { return Circle::iRadius; }
};

