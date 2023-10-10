#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 蜀・｡晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class CircleCollider : public BaseCollider, public Circle
{
private: // 繧ｨ繧､繝ｪ繧｢繧ｹ
	// DirectX::繧堤怐逡･
	using XMVECTOR = DirectX::XMVECTOR;
public:
	CircleCollider(const Vec2 center = { 0,0 }, float radius = 1.0f)
	{
		Circle::center = center;
		Circle::iRadius = radius;

		// 逅・ｽ｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_CIRCLE;

		is2D_ = true;
	}

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;

	inline void SetRadius(float radius) { Circle::iRadius = radius; }
	inline float GetRadius() { return Circle::iRadius; }
};

