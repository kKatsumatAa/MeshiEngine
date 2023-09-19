#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// �~�Փ˔���I�u�W�F�N�g
/// </summary>
class RectangleCollider : public BaseCollider, public MyRectangle
{
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;
public:
	RectangleCollider(Vec2 leftUp = { -1.0f,-1.0f },
		Vec2 leftDown = { -1.0f,1.0f },
		Vec2 rightUp = { 1.0f,-1.0f },
		Vec2 rightDown = { 1.0f,1.0f })
	{
		MyRectangle::leftUp = leftUp;
		MyRectangle::leftDown = leftDown;
		MyRectangle::rightUp = rightUp;
		MyRectangle::rightDown = rightDown;

		// ��`�`����Z�b�g
		shapeType_ = COLLISIONSHAPE_RECTANGLE;

		is2D_ = true;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;
};

