#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"


/// <summary>
/// 髱｢陦晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class TriangleCollider : public BaseCollider, public Triangle
{
private: // 繧ｨ繧､繝ｪ繧｢繧ｹ
	// DirectX::繧堤怐逡･
	using XMVECTOR = DirectX::XMVECTOR;
public:
	TriangleCollider(const DirectX::XMVECTOR& p0 = { 0,0,0,0 }, const DirectX::XMVECTOR& p1 = { 0,0,0,0 }, const DirectX::XMVECTOR& p2 = { 0,0,0,0 }, const XMVECTOR& normal = { 0, 0, 0, 0 }) :
		p0_(p0), p1_(p1), p2_(p2), normal_(normal)
	{
		// 蠖｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) { p0_ = p0; p1_ = p1; p2_ = p2; }

private:
	//鬆らせ蠎ｧ讓・縺､
	DirectX::XMVECTOR p0_;
	DirectX::XMVECTOR p1_;
	DirectX::XMVECTOR p2_;
	//豕慕ｷ壹・繧ｯ繝医Ν
	DirectX::XMVECTOR normal_;
};

