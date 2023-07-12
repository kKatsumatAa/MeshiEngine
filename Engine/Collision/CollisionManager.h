#pragma once

#include<forward_list>
#include"CollisionPrimitive.h"
#include"RaycastHit.h"
#include"QueryCallback.h"

#include<d3d12.h>

class BaseCollider;

//Object�������Ă���R���C�_�[�����̃N���X�̕ϐ��ɓo�^���Ďg��
class CollisionManager
{
private:
	CollisionManager() = default;
	~CollisionManager() = default;

private:
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*> colliders_;



public://�ÓI�����o�֐�
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

	static CollisionManager* GetInstance();

private:
	

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

	/// <summary>
	/// ���C�L���X�g���葮���w���
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="attribute">���葮��</param>
	/// <param name="hitInfo">�Փˏ��</param>
	/// <param name="maxDistance">�ő勗��</param>
	/// <returns></returns>
	bool Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	///�@���C�L���X�g�֗̕���
	/// </summary>
	/// <param name="pos">�I�_</param>
	/// <param name="oldPos">�n�_</param>
	/// <param name="attribute">���葮��</param>
	/// <param name="info"></param>
	/// <returns></returns>
	bool RaycastUtil(const Vec3& endPos, const Vec3& startPos, uint16_t attribute, RaycastHit* info);

	/// <summary>
	/// ���ɂ��ՓˑS����
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="callBack"></param>
	/// <param name="attribute"></param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callBack, uint16_t attribute = (uint16_t)0xffffffff);

	void CheckAllCollisions();

	void Initialize();
};

