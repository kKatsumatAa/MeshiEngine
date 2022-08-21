#pragma once
#include"Vec3.h"
#include"Vec2.h"
#include"CollisionConfig.h"
#include <cstdint>

/// <summary>
/// �Փ˔���I�u�W�F�N�g
/// </summary>
class Collider
{
private:
	

	//�Փˑ���(����)
	uint32_t collisionAttribute_ = 0xffffffff;
	//�Փˑ���(����)
	uint32_t collisionMask_ = 0xffffffff;


protected:


public:
	bool isBoss = false;
	int coolTime = 0;

	int HP=0;

	float radius_ = 5.0f;
	//���b�N�I�����[�h���ǂ���(player�p�������蔻��̎��Ɏg�p)
	bool isLockOn = false;
	//���������b�N�I�����Ă���G�̐��iplayer�p�j
	int isLockNum = 0;
	//���S�t���O
	bool isDead = false;
	//���b�N�I������Ă��邩�ǂ����i�G�p�j
	int isLockOned = 0;

	float GetRadius();
	void SetRadius(const float& radius);

	//�Փˎ��ɌĂ΂��
	virtual void OnCollision() = 0;
	virtual void OnCollision2() = 0;
	//world���W���Q�b�g
	virtual Vec3 GetWorldPos() = 0;

	//�Փ˗p�r�b�g�n
	//����
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(const uint32_t& bit);
	//�G
	uint32_t GetCollisionMask();
	void SetCollisionMask(const uint32_t& bit);
};

