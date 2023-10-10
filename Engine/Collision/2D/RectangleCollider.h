#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// 蜀・｡晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class RectangleCollider : public BaseCollider, public MyRectangle
{
private: // 繧ｨ繧､繝ｪ繧｢繧ｹ
	// DirectX::繧堤怐逡･
	using XMVECTOR = DirectX::XMVECTOR;
public:
	RectangleCollider(Vec2 leftUp = { -1.0f,-1.0f },
		Vec2 leftDown = { -1.0f,1.0f },
		Vec2 rightUp = { 1.0f,-1.0f },
		Vec2 rightDown = { 1.0f,1.0f })
	{
		MyRectangle::iLeftUp = leftUp;
		MyRectangle::iLeftDown = leftDown;
		MyRectangle::iRightUp = rightUp;
		MyRectangle::iRightDown = rightDown;

		// 遏ｩ蠖｢蠖｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_RECTANGLE;

		is2D_ = true;
	}

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;
};

