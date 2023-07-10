#pragma once

#include <DirectXMath.h>
#include "CollisionAttribute.h"

class Object;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo() { ; }
	~CollisionInfo() { ; }


public:
	CollisionInfo(Object* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}

	// 衝突相手のオブジェクト
	Object* object_ = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider_ = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter_;
};

