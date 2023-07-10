#pragma once

#include <DirectXMath.h>
#include "CollisionAttribute.h"

class Object;
class BaseCollider;

/// <summary>
/// �Փˏ��
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo() { ; }
	~CollisionInfo() { ; }


public:
	CollisionInfo(Object* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}

	// �Փˑ���̃I�u�W�F�N�g
	Object* object_ = nullptr;
	// �Փˑ���̃R���C�_�[
	BaseCollider* collider_ = nullptr;
	// �Փ˓_
	DirectX::XMVECTOR inter_;
};

