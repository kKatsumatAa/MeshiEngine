#pragma once
#include "CollisionPrimitive.h"

/// <summary>
/// 当たり判定ヘルパークラス
/// </summary>
class Collision
{
public:
	/// <summary>
	/// 球同士の当たり判定
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="sphere"></param>
	/// <returns></returns>
	static bool CheckSphere2Sphere(const Sphere& sphere, const Sphere& sphere2, DirectX::XMVECTOR* pInter);
	/// <summary>
	/// 球と平面の当たり判定
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="plane"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane,
		DirectX::XMVECTOR* pInter = nullptr);

	/// <summary>
	/// 点と三角形の最近接点を求める
	/// </summary>
	/// <param name="point"></param>
	/// <param name="triangle"></param>
	/// <param name="closest"></param>
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point,
		const Triangle& triangle, DirectX::XMVECTOR* pClosest);

	/// <summary>
	/// 球と法線付き三角形の当たり判定チェック
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="triangle"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle,
		DirectX::XMVECTOR* pInter = nullptr);

	/// <summary>
	/// レイと平面の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="plane"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane,
		float* pDistance = nullptr, DirectX::XMVECTOR* pInter = nullptr);

	/// <summary>
	/// レイと法線付き三角形の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="triangle"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle,
		float* pDistance = nullptr, DirectX::XMVECTOR* pInter = nullptr);

	/// <summary>
	/// レイと球の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="sphere"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere,
		float* pDistance = nullptr, DirectX::XMVECTOR* pInter = nullptr);
};

