#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// �ʏՓ˔���I�u�W�F�N�g
/// </summary>
class PlaneCollider : public BaseCollider, public Plane
{
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;
public:
	PlaneCollider(XMVECTOR normal = { 0,0,0,0 }, float distance = 1.0f) :
		normal(normal), distance(distance)
	{
		// ���`����Z�b�g
		shapeType = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetNormal(DirectX::XMVECTOR normal) { this->normal = normal; }
	inline void SetDistance(float distance) { this->distance = distance; }

private:
	//�@���x�N�g��
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//���_(0,0,0)����̋���
	float distance = 0.0f;
};

