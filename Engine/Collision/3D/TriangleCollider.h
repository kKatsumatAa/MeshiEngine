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
	TriangleCollider(const DirectX::XMVECTOR& p0 = { 0,0,0,0 }, const DirectX::XMVECTOR& p1 = { 0,0,0,0 }, const DirectX::XMVECTOR& p2 = { 0,0,0,0 }, const XMVECTOR& normal = { 0, 0, 0, 0 }) :
		p0_(p0), p1_(p1), p2_(p2), normal_(normal)
	{
		// �`����Z�b�g
		shapeType_ = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal_ = normal; }
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) { p0_ = p0; p1_ = p1; p2_ = p2; }

private:
	//���_���W3��
	DirectX::XMVECTOR p0_;
	DirectX::XMVECTOR p1_;
	DirectX::XMVECTOR p2_;
	//�@���x�N�g��
	DirectX::XMVECTOR normal_;
};

