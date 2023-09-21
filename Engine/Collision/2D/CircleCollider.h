#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// �~�Փ˔���I�u�W�F�N�g
/// </summary>
class CircleCollider : public BaseCollider, public Circle
{
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;
public:
	CircleCollider(const Vec2 center = { 0,0 }, float radius = 1.0f)
	{
		Circle::center = center;
		Circle::radius = radius;

		// ���`����Z�b�g
		shapeType_ = COLLISIONSHAPE_CIRCLE;

		is2D_ = true;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetRadius(float radius) { Circle::radius = radius; }
	inline float GetRadius() { return Circle::radius; }
};

