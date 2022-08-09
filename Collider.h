#pragma once
#include"Vec3.h"
#include"CollisionConfig.h"
#include <cstdint>

/// <summary>
/// �Փ˔���I�u�W�F�N�g
/// </summary>
class Collider
{
private:
	float radius_ = 5.0f;

	//�Փˑ���(����)
	uint32_t collisionAttribute_ = 0xffffffff;
	//�Փˑ���(����)
	uint32_t collisionMask_ = 0xffffffff;

public:
	float GetRadius();
	void SetRadius(const float& radius);

	//�Փˎ��ɌĂ΂��
	virtual void OnCollision() = 0;
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

