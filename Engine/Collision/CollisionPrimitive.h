#pragma once
///<summary>
/// 蠖薙◆繧雁愛螳壹・繝ｪ繝溘ユ繧｣繝・
/// </summary>

#include<DirectXMath.h>
#include"Vec2.h"
#include"Vec3.h"

/// <summary>
/// 逅・
/// </summary>
struct Sphere
{
public:
	virtual ~Sphere() { ; }

	//荳ｭ蠢・ｺｧ讓・
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//蜊雁ｾ・
	float iRadius = 1.0f;
};

/// <summary>
/// 蟷ｳ髱｢
/// </summary>
struct Plane
{
public:
	virtual ~Plane() { ; }

public:
	//豕慕ｷ壹・繧ｯ繝医Ν
	DirectX::XMVECTOR iNormal = { 0,1,0,0 };
	//蜴溽せ(0,0,0)縺九ｉ縺ｮ霍晞屬
	float iDistance = 0.0f;
};

/// <summary>
/// 豕慕ｷ壻ｻ倥″荳芽ｧ貞ｽ｢・域凾險亥屓繧翫′陦ｨ髱｢・・
/// </summary>
struct Triangle
{
	//鬆らせ蠎ｧ讓・縺､
	DirectX::XMVECTOR iP0;
	DirectX::XMVECTOR iP1;
	DirectX::XMVECTOR iP2;
	//豕慕ｷ壹・繧ｯ繝医Ν
	DirectX::XMVECTOR iNormal;

public:
	virtual ~Triangle() { ; }

	/// <summary>
	/// 豕慕ｷ壹・險育ｮ・
	/// </summary>
	void ComputeNormal();
};

/// <summary>
/// 繝ｬ繧､
/// </summary>
struct Ray
{
public:
	virtual ~Ray() { ; }

public:
	//隕也せ蠎ｧ讓・
	DirectX::XMVECTOR start = { 0,0,0,1.0f };
	//譁ｹ蜷・
	DirectX::XMVECTOR dir = { 1.0f,0,0,0 };
};


// 2D

/// <summary>
/// 蜀・
/// </summary>
struct Circle
{
public:
	virtual ~Circle() { ; }

public:
	//荳ｭ蠢・ｺｧ讓・
	Vec2 center = { 0,0 };
	//蜊雁ｾ・
	float iRadius = 1.0f;
};

/// <summary>
/// 遏ｩ蠖｢
/// </summary>
struct MyRectangle
{
public:
	virtual ~MyRectangle() { ; }

public:
	//蜷・らせ
	Vec2 iLeftUp = { -1.0f,-1.0f };
	Vec2 iLeftDown = { -1.0f,1.0f };
	Vec2 iRightUp = { 1.0f,-1.0f };
	Vec2 iRightDown = { 1.0f,1.0f };
};


