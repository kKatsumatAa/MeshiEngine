/*
* @file RectangleCollider.h
* @brief 矩形のコライダー
*/

#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include <DirectXMath.h>

/// <summary>
/// 円衝突判定オブジェクト
/// </summary>
class RectangleCollider : public BaseCollider, public MyRectangle
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	//コンストラクタ
	RectangleCollider(Vec2 leftUp = { -1.0f,-1.0f },
		Vec2 leftDown = { -1.0f,1.0f },
		Vec2 rightUp = { 1.0f,-1.0f },
		Vec2 rightDown = { 1.0f,1.0f })
	{
		MyRectangle::iLeftUp = leftUp;
		MyRectangle::iLeftDown = leftDown;
		MyRectangle::iRightUp = rightUp;
		MyRectangle::iRightDown = rightDown;

		// 矩形形状をセット
		shapeType_ = COLLISIONSHAPE_RECTANGLE;

		is2D_ = true;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
};

