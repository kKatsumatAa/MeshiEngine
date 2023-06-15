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
		p0(p0), p1(p1), p2(p2), normal(normal)
	{
		// �`����Z�b�g
		shapeType = COLLISIONSHAPE_TRIANGLE;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	inline void SetNormal(const DirectX::XMVECTOR& normal) { normal = normal; }
	inline void SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) { p0 = p0; p1 = p1; p2 = p2; }

private:
	//���_���W3��
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//�@���x�N�g��
	DirectX::XMVECTOR normal;
};

