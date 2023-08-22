#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"
#include "Character.h"


class Enemy :
	public Character
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.8f;
	bool isCantMove = false;

	//�E�F�[�u�ԍ�
	int32_t waveNum_ = -1;

	//�������v���C���[�Ɍ����邽�߂̃N�H�[�^�j�I��
	Quaternion directionQua_;
	//���������̃x�N�g��
	Vec3 directionVec = { 0,0,1.0f };
	//��]�̎���
	float directionRotTime = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	float damageCoolTime = 0;
	float velocityLength = 0;

	const int8_t HP_TMP_ = 3;

	const float KNOCK_BACK_POW_ = 0.165f;

	const float DISSOLVE_POW_ = 0.6f;

	const float WEAPON_FALL_VEL_EXTEND_ = 1.6f;


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon);

private:
	//�ړ�
	void Move();

	//�e���m�b�N�o�b�N���ė��Ƃ�
	void KnockBack(const CollisionInfo& info);

	//��e���̃p�[�e�B�N��
	void DamageParticle(const CollisionInfo& info);

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon);

	void Update() override;

	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;

public:
	int32_t GetWaveNum() { return waveNum_; }
};

