#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 面衝突判定オブジェクト
/// </summary>
class PlaneCollider : public BaseCollider, public Plane
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	PlaneCollider(XMVECTOR normal = { 0,0,0,0 }, float distance = 1.0f) :
		normal(normal), distance(distance)
	{
		// 球形状をセット
		shapeType = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline void SetNormal(DirectX::XMVECTOR normal) { this->normal = normal; }
	inline void SetDistance(float distance) { this->distance = distance; }

private:
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance = 0.0f;
};

