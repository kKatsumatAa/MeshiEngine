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
	CircleCollider(const Vec2 center = { 0,0 }, float radius = 1.0f)
	{
		Circle::center = center;
		Circle::radius = radius;

		// 球形状をセット
		shapeType_ = COLLISIONSHAPE_CIRCLE;

		is2D_ = true;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline void SetRadius(float radius) { Circle::radius = radius; }
	inline float GetRadius() { return Circle::radius; }
};

