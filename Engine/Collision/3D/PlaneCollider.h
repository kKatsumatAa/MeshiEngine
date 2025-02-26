/*
* @file PlaneCollider.h
* @brief 面コライダー
*/


#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"


/// <summary>
/// 面衝突判定オブジェクト
/// </summary>
class PlaneCollider : public BaseCollider, public Plane
{
public:
	const Vec3 ROT_VEC_TMP_ = { 0,0,-1.0f };

private:
	//法線ベクトル
	Vec4 normal_ = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance_ = 0.0f;

private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	//コンストラクタ
	PlaneCollider(const XMVECTOR& normal = { 0,0,0,0 }, float distance = 1.0f)
	{
		normal_ = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2],normal.m128_f32[3] };
		distance_ = distance;

		// 形状をセット
		shapeType_ = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//法線セット
	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2],normal.m128_f32[3] }; }
	//距離セット
	inline void SetDistance(float distance) { distance_ = distance; }
};

