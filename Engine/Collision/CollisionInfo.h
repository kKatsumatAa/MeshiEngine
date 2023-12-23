/*
* @file CollisionInfo.h
* @brief 衝突相手の情報
*/

#pragma once
#include "CollisionAttribute.h"

#pragma warning(push)
//c5267警告を無視
#pragma warning(disable:5267)
#include <DirectXMath.h>
#pragma warning(pop)

class IObject;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	// 衝突相手のオブジェクト
	IObject* object_ = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider_ = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter_;

public:
	//コンストラクタ
	CollisionInfo() { ; }
	//デストラクタ
	~CollisionInfo() { ; }

public:
	//コンストラクタ（引数アリ）
	CollisionInfo(IObject* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}
};

