#pragma once

#include <DirectXMath.h>

class Object;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(Object* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object = object;
		collider = collider;
		inter = inter;
	}

	// 衝突相手のオブジェクト
	Object* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter;
};

