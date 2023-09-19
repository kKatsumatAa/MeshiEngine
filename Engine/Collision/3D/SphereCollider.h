#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 球衝突判定オブジェクト
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
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

	inline void SetRadius(float radius) { radius_ = radius; }
	inline float GetRadius() { return radius_; }

private:
	// オブジェクト中心からのオフセット
	XMVECTOR offset_;
	// 半径
	float radius_;
};

