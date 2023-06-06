#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// �ʏՓ˔���I�u�W�F�N�g
/// </summary>
class TriangleCollider : public BaseCollider, public Triangle
{
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;
public:
	TriangleCollider(DirectX::XMVECTOR p0 = { 0,0,0,0 }, DirectX::XMVECTOR p1 = { 0,0,0,0 }, DirectX::XMVECTOR p2 = { 0,0,0,0 }, XMVECTOR normal = { 0, 0, 0, 0 }) :
		p0(p0), p1(p1), p2(p2), normal(normal)
	{
		// �`����Z�b�g
		shapeType = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetNormal(DirectX::XMVECTOR normal) { this->normal = normal; }
	inline void SetPos(DirectX::XMVECTOR p0, DirectX::XMVECTOR p1, DirectX::XMVECTOR p2) { this->p0 = p0; this->p1 = p1; this->p2 = p2; }

private:
	//���_���W3��
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//�@���x�N�g��
	DirectX::XMVECTOR normal;
};

