/*
* @file TriangleCollider.h
* @brief 三角のコライダー
*/

#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"


/// <summary>
/// 面衝突判定オブジェクト
/// </summary>
class TriangleCollider : public BaseCollider, public Triangle
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;

public:
	const float ANGLE_1_ = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
	const float ANGLE_2_ = PI * 2.0f;
	const float ANGLE_3_ = PI * 2.0f + PI * 2.0f / 3.0f;

private:
	//頂点座標3つ
	Vec4 p0_;
	Vec4 p1_;
	Vec4 p2_;
	//法線ベクトル
	Vec4 normal_;

public:
	//コンストラクタ
	TriangleCollider(const DirectX::XMVECTOR& p0 = { 0,0,0,0 }, const DirectX::XMVECTOR& p1 = { 0,0,0,0 }, const DirectX::XMVECTOR& p2 = { 0,0,0,0 }, const XMVECTOR& normal = { 0, 0, 0, 0 })
	{
		p0_ = { p0.m128_f32[0],p0.m128_f32[1],p0.m128_f32[2],p0.m128_f32[3] };
		p1_ = { p1.m128_f32[0],p1.m128_f32[1],p1.m128_f32[2],p1.m128_f32[3] };
		p2_ = { p2.m128_f32[0],p2.m128_f32[1],p2.m128_f32[2],p2.m128_f32[3] };
		normal_ = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2],normal.m128_f32[3] };

		// 形状をセット
		shapeType_ = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//法線セット
	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2],normal.m128_f32[3] }; }
	//位置セット
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2);
};

