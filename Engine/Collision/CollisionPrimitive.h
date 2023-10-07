#pragma once
///<summary>
/// 当たり判定プリミティブ
/// </summary>

#include<DirectXMath.h>
#include"Vec2.h"
#include"Vec3.h"

/// <summary>
/// 球
/// </summary>
struct Sphere
{
public:
	virtual ~Sphere() { ; }

	//中心座標
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//半径
	float radius = 1.0f;
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
public:
	virtual ~Plane() { ; }

public:
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance = 0.0f;
};

/// <summary>
/// 法線付き三角形（時計回りが表面）
/// </summary>
struct Triangle
{
	//頂点座標3つ
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//法線ベクトル
	DirectX::XMVECTOR normal;

public:
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
	virtual ~Ray() { ; }

public:
	//視点座標
	DirectX::XMVECTOR start = { 0,0,0,1.0f };
	//方向
	DirectX::XMVECTOR dir = { 1.0f,0,0,0 };
};


// 2D

/// <summary>
/// 円
/// </summary>
struct Circle
{
public:
	virtual ~Circle() { ; }

public:
	//中心座標
	Vec2 center = { 0,0 };
	//半径
	float radius = 1.0f;
};

/// <summary>
/// 矩形
/// </summary>
struct MyRectangle
{
public:
	virtual ~MyRectangle() { ; }

public:
	//各頂点
	Vec2 leftUp = { -1.0f,-1.0f };
	Vec2 leftDown = { -1.0f,1.0f };
	Vec2 rightUp = { 1.0f,-1.0f };
	Vec2 rightDown = { 1.0f,1.0f };
};


