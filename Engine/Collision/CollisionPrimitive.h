/*
* @file CollisionPrimitive.h
* @brief 当たり判定プリミティブ
*/

#pragma once
#include<DirectXMath.h>
#include"Vec2.h"
#include"Vec3.h"

/// <summary>
/// 球
/// </summary>
struct Sphere
{
public:
	//中心座標
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//半径
	float iRadius = 1.0f;

public:
	//デストラクタ
	virtual ~Sphere() { ; }
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
public:
	//法線ベクトル
	DirectX::XMVECTOR iNormal = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float iDistance = 0.0f;

public:
	//デストラクタ
	virtual ~Plane() { ; }
};

/// <summary>
/// 法線付き三角形（時計回りが表面）
/// </summary>
struct Triangle
{
	//頂点座標3つ
	DirectX::XMVECTOR iP0;
	DirectX::XMVECTOR iP1;
	DirectX::XMVECTOR iP2;
	//法線ベクトル
	DirectX::XMVECTOR iNormal;

public:
	//デストラクタ
	virtual ~Triangle() { ; }

	/// <summary>
	/// 法線の計算
	/// </summary>
	void ComputeNormal();
};

/// <summary>
/// レイ
/// </summary>
struct Ray
{
public:
	//視点座標
	DirectX::XMVECTOR start = { 0,0,0,1.0f };
	//方向
	DirectX::XMVECTOR dir = { 1.0f,0,0,0 };

public:
	//デストラクタ
	virtual ~Ray() { ; }
};


// 2D------------------------------------------------------------------------
/// <summary>
/// 円
/// </summary>
struct Circle
{
public:
	//中心座標
	Vec2 center = { 0,0 };
	//半径
	float iRadius = 1.0f;

public:
	//デストラクタ
	virtual ~Circle() { ; }
};

/// <summary>
/// 矩形
/// </summary>
struct MyRectangle
{
public:
	//各頂点
	Vec2 iLeftUp = { -1.0f,-1.0f };
	Vec2 iLeftDown = { -1.0f,1.0f };
	Vec2 iRightUp = { 1.0f,-1.0f };
	Vec2 iRightDown = { 1.0f,1.0f };

public:
	//デストラクタ
	virtual ~MyRectangle() { ; }
};


