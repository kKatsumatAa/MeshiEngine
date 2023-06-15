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
		p0(p0), p1(p1), p2(p2), normal(normal)
	{
		// 形状をセット
		shapeType = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal = normal; }
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) { p0 = p0; p1 = p1; p2 = p2; }

private:
	//頂点座標3つ
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//法線ベクトル
	DirectX::XMVECTOR normal;
};

