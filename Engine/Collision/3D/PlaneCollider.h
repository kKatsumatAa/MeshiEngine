#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"


/// <summary>
/// 髱｢陦晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class PlaneCollider : public BaseCollider, public Plane
{
private: // 繧ｨ繧､繝ｪ繧｢繧ｹ
	// DirectX::繧堤怐逡･
	using XMVECTOR = DirectX::XMVECTOR;
public:
	PlaneCollider(const XMVECTOR& normal = { 0,0,0,0 }, float distance = 1.0f) :
		normal_(normal), distance_(distance)
	{
		// 蠖｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetDistance(float distance) { distance_ = distance; }

private:
	//豕慕ｷ壹・繧ｯ繝医Ν
	DirectX::XMVECTOR normal_ = { 0,1,0,0 };
	//蜴溽せ(0,0,0)縺九ｉ縺ｮ霍晞屬
	float distance_ = 0.0f;
};

