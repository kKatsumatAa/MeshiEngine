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
	static bool CheckSphere2Sphere(const Sphere& sphere, const Sphere& sphere2, DirectX::XMVECTOR* inter
		, DirectX::XMVECTOR* reject = nullptr);
	/// <summary>
	/// 球と平面の当たり判定
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="plane"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane,
		DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// 点と三角形の最近接点を求める
	/// </summary>
	/// <param name="point"></param>
	/// <param name="triangle"></param>
	/// <param name="closest"></param>
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point,
		const Triangle& triangle, DirectX::XMVECTOR* closest);

	/// <summary>
	/// 球と法線付き三角形の当たり判定チェック
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="triangle"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// レイと平面の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="plane"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// レイと法線付き三角形の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="triangle"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// レイと球の当たり判定
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="sphere"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
};

