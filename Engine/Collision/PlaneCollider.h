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
	PlaneCollider(const XMVECTOR& normal = { 0,0,0,0 }, float distance = 1.0f) :
		normal_(normal), distance_(distance)
	{
		// �`����Z�b�g
		shapeType_ = COLLISIONSHAPE_PLANE;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetDistance(float distance) { distance_ = distance; }

private:
	//�@���x�N�g��
	DirectX::XMVECTOR normal_ = { 0,1,0,0 };
	//���_(0,0,0)����̋���
	float distance_ = 0.0f;
};

