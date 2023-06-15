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
	PlaneCollider(const XMVECTOR& normal = { 0,0,0,0 }, float distance = 1.0f) :
		normal_(normal), distance_(distance)
	{
		// 形状をセット
		shapeType_ = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetDistance(float distance) { distance_ = distance; }

private:
	//法線ベクトル
	DirectX::XMVECTOR normal_ = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance_ = 0.0f;
};

