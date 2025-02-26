/*
* @file RaycastHit.h
* @brief レイの衝突したときのデータ
*/

#pragma once
#include "BaseCollider.h"

class IObject3D;

/// <summary>
/// レイキャストによる情報を得る為の構造体（CollisionInfoのようなもの）
/// </summary>
struct RaycastHit
{
	// 衝突相手のオブジェクト
	IObject3D* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter;
	// 衝突点までの距離
	float distance = 0.0f;
};

