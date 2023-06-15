#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 面衝突判定オブジェクト
/// </summary>
class TriangleCollider : public BaseCollider, public Triangle
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	TriangleCollider(const DirectX::XMVECTOR& p0 = { 0,0,0,0 }, const DirectX::XMVECTOR& p1 = { 0,0,0,0 }, const DirectX::XMVECTOR& p2 = { 0,0,0,0 }, const XMVECTOR& normal = { 0, 0, 0, 0 }) :
		p0_(p0), p1_(p1), p2_(p2), normal_(normal)
	{
		// 形状をセット
		shapeType_ = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) { p0_ = p0; p1_ = p1; p2_ = p2; }

private:
	//頂点座標3つ
	DirectX::XMVECTOR p0_;
	DirectX::XMVECTOR p1_;
	DirectX::XMVECTOR p2_;
	//法線ベクトル
	DirectX::XMVECTOR normal_;
};

