#pragma once

#include <DirectXMath.h>

class Object;
class BaseCollider;

/// <summary>
/// �Փˏ��
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(Object* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object = object;
		collider = collider;
		inter = inter;
	}

	// �Փˑ���̃I�u�W�F�N�g
	Object* object = nullptr;
	// �Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	// �Փ˓_
	DirectX::XMVECTOR inter;
};

