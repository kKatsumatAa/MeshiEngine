#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

/// <summary>
/// 逅・｡晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{
private: // 繧ｨ繧､繝ｪ繧｢繧ｹ
	// DirectX::繧堤怐逡･
	using XMVECTOR = DirectX::XMVECTOR;
public:
	SphereCollider(const XMVECTOR& offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset_(offset), radius_(radius)
	{
		// 逅・ｽ｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;

	inline void SetRadius(float radius) { radius_ = radius; }
	inline float GetRadius() { return radius_; }

private:
	// 繧ｪ繝悶ず繧ｧ繧ｯ繝井ｸｭ蠢・°繧峨・繧ｪ繝輔そ繝・ヨ
	XMVECTOR offset_;
	// 蜊雁ｾ・
	float radius_;
};

