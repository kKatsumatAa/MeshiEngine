#pragma once

#include<forward_list>
#include"CollisionPrimitive.h"
#include"RaycastHit.h"

#include<d3d12.h>

class BaseCollider;

//Object�������Ă���R���C�_�[�����̃N���X�̕ϐ��ɓo�^���Ďg��
class CollisionManager 
{
private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;


private:
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*> colliders_;



public://�ÓI�����o�֐�
	static CollisionManager* GetInstance();

public://�֐�
	/// <summary>
	/// �R���C�_�[�̒ǉ�
	/// </summary>
	/// <param name="collider"></param>
	inline void AddCollider(BaseCollider* collider)
	{
		colliders_.push_front(collider);
	}

	/// <summary>
	/// �R���C�_�[�̍폜
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider)
	{
		colliders_.remove(collider);
	}

	/// <summary>
	/// ���C�L���X�g
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="hitInfo">�Փˏ��</param>
	/// <param name="maxDistance">�ő勗��</param>
	/// <returns>���C���C�ӂ̃R���C�_�[�ƌ����ꍇ��true</returns>
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	void CheckAllCollisions();

	void Initialize();
};

