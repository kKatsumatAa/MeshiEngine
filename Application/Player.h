#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"

class PlayerAttackState;

class Player :
	public Object
{
private:
	const float ANGLE_VEL_EXTEND_ = 0.003f;
	Vec3 cameraRot_ = { 0,0,0 };

	Vec3 frontVec_ = { 0,0,1.0f };
	Vec3 upVec_ = { 0,1.0f,0 };
	Vec3 rightVec_ = { 0,0,0 };

	//�U������
	bool isAttacking_ = false;
	//�U���\��
	bool isCanAttack_ = false;

	//�U�����͂�����
	const float attackLength_ = 20.0f;

	//�U���̃X�e�[�g
	std::unique_ptr<PlayerAttackState> attackState_ = nullptr;

public:


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Player> Create();

private:
	//�ړ�
	void Move();
	//������ς���
	void DirectionUpdate();

public:
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	void SetIsCanAttack(bool is) { isCanAttack_ = is; }

	bool GetIsAttacking() { return isAttacking_; }
	bool GetIsCanAttack() { return isCanAttack_; }

	float GetAttackLength() { return attackLength_; }

public:

	bool Initialize() override;

	void Update() override;

	void Draw() override;

	//�U���̃X�e�[�g�ύX
	void ChangeAttackState(std::unique_ptr<PlayerAttackState> state);

	void OnCollision(const CollisionInfo& info) override;
};

