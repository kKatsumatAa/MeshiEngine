#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "PlayerHandManager.h"
#include "Gun.h"
#include "Character.h"


class PlayerAttackState;
class PlayerState;

class Player :
	public Character
{
private:
	const int32_t HP_TMP_ = 1;

	const float VELOCITY_TMP_ = 1.15f;
	const float ANGLE_VEL_EXTEND_ = 0.003f;
	//�}�E�X�ł̃Q�[���X�s�[�h���Z�{��
	const float MOUSE_GAME_VEL_EXTEND_ = 0.001f;

	Vec3 cameraRot_ = { 0,0,0 };

	Vec3 frontVec_ = { 0,0,1.0f };
	Vec3 upVec_ = { 0,1.0f,0 };
	Vec3 rightVec_ = { 0,0,0 };

	//�U������
	bool isAttacking_ = false;
	//�U���\��
	bool isTarget_ = false;

	//�U�����͂�����
	const float ATTACK_LENGTH_ = 20.0f;

	//�}�E�X�̃J�����ړ��X�s�[�h
	const float MOUSE_VELOCITY_TMP_ = 0.5f;
	//�ړ��ɂ��Q�[���X�s�[�h���Z�̔{��
	const float MOVE_ADD_VEL_EXTEND_ = 0.13f;

	//��̃}�l�[�W���[�N���X
	std::unique_ptr<PlayerHandManager> handManager_ = nullptr;

	//��Ԃ̃X�e�[�g
	std::unique_ptr<PlayerState> state_ = nullptr;

	//
	int32_t deadTimer_ = 70;
	bool isDead_ = false;

	const float FALL_VEL_POW_ = 8.0f;

public:


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Player> Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

private:
	//�ړ�
	void Move();
	//������ς���
	void DirectionUpdate();
	//�|���ꂽ���ƂɌĂяo��
	void Dead(const CollisionInfo& info);

public:
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	void SetIsTarget(bool is) { isTarget_ = is; }

	bool GetIsAttacking() { return isAttacking_; }
	bool GetIsCanAttack() { return isTarget_; }

	float GetAttackLength() { return ATTACK_LENGTH_; }

	//��̃}�l�[�W���[
	PlayerHandManager* GetHandManager() { return handManager_.get(); }

public:
	bool Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon);

	void Update() override;

	void Draw() override;

	void ChangePlayerState(std::unique_ptr<PlayerState> state);

	void OnCollision(const CollisionInfo& info) override;

public:
	void ThrowWeapon();
};

