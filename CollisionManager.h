#pragma once

#include<forward_list>

class BaseCollider;

//Object�������Ă���R���C�_�[�����̃N���X�̕ϐ��ɓo�^���Ďg��
class CollisionManager 
{
private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*> colliders;



public://�ÓI�����o�֐�
	static CollisionManager* GetInstance();

public://�֐�
	/// <summary>
	/// �R���C�_�[�̒ǉ�
	/// </summary>
	/// <param name="collider"></param>
	inline void AddCollider(BaseCollider* collider)
	{
		colliders.push_front(collider);
	}

	/// <summary>
	/// �R���C�_�[�̍폜
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider)
	{
		colliders.remove(collider);
	}


	void CheckAllCollisions();

	void Initialize();
};

