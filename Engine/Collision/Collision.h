#pragma once
#include "CollisionPrimitive.h"

/// <summary>
/// �����蔻��w���p�[�N���X
/// </summary>
class Collision
{
public:
	/// <summary>
	/// �����m�̓����蔻��
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="sphere"></param>
	/// <returns></returns>
	static bool CheckSphere2Sphere(const Sphere& sphere, const Sphere& sphere2, DirectX::XMVECTOR* inter
		, DirectX::XMVECTOR* reject = nullptr);
	/// <summary>
	/// ���ƕ��ʂ̓����蔻��
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="plane"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane,
		DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// �_�ƎO�p�`�̍ŋߐړ_�����߂�
	/// </summary>
	/// <param name="point"></param>
	/// <param name="triangle"></param>
	/// <param name="closest"></param>
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point,
		const Triangle& triangle, DirectX::XMVECTOR* closest);

	/// <summary>
	/// ���Ɩ@���t���O�p�`�̓����蔻��`�F�b�N
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="triangle"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// ���C�ƕ��ʂ̓����蔻��
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="plane"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// ���C�Ɩ@���t���O�p�`�̓����蔻��
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="triangle"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// ���C�Ƌ��̓����蔻��
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="sphere"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere,
		float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

public://2D
	/// <summary>
	/// �~�Ɖ~�̔���
	/// </summary>
	/// <param name="circle"></param>
	/// <param name="circle2"></param>
	/// <param name="inter"></param>
	/// <param name="reject"></param>
	/// <returns></returns>
	static bool CheckCircle2Circle(const Circle& circle, const Circle& circle2,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);


	//static bool CheckRectangle2Rectangle(const MyRectangle& rectangle, const MyRectangle& rectangle2,
	//	DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
};

