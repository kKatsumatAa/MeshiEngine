#pragma once
#include "CollisionAttribute.h"

#pragma warning(push)
//c5267隴ｦ蜻翫ｒ辟｡隕・
#pragma warning(disable:5267)
#include <DirectXMath.h>
#pragma warning(pop)

class IObject;
class BaseCollider;

/// <summary>
/// 陦晉ｪ∵ュ蝣ｱ
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo() { ; }
	~CollisionInfo() { ; }


public:
	CollisionInfo(IObject* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}

	// 陦晉ｪ∫嶌謇九・繧ｪ繝悶ず繧ｧ繧ｯ繝・
	IObject* object_ = nullptr;
	// 陦晉ｪ∫嶌謇九・繧ｳ繝ｩ繧､繝繝ｼ
	BaseCollider* collider_ = nullptr;
	// 陦晉ｪ∫せ
	DirectX::XMVECTOR inter_;
};

