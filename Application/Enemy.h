#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Enemy :
	public Object
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 1.0f;
	bool isCantMove = false;

	//�������v���C���[�Ɍ����邽�߂̃N�H�[�^�j�I��
	Quaternion directionQua_;
	//���������̃x�N�g��
	Vec3 directionVec = { 0,0,1.0f };
	//��]�̎���
	float directionRotTime = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	//hp
	int8_t hp = 3;
	float damageCoolTime = 0;
	float velocityLength = 0;

public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create();

private:
	//�ړ�
	void Move();

public:

	bool Initialize() override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

