#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"
#include "Gun.h"
#include "Character.h"


class EnemyState;

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
	//��]�̎���
	float directionRotTime_ = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	Vec3 directionVec_;

	float damageCoolTime_ = 0;
	float velocityLength_ = 0;

	const int8_t HP_TMP_ = 3;

	const float KNOCK_BACK_POW_ = 0.365f;

	const float DISSOLVE_POW_ = 0.6f;

	const float WEAPON_FALL_VEL_EXTEND_ = 1.6f;

	//�o�����o
	const int32_t LIGHT_INDEX_INIT_ = -1;
	int32_t lightIndexTmp_ = LIGHT_INDEX_INIT_;
	DirectX::XMFLOAT3 EMERGE_COL_ = { 1.0f,0,0 };

	//�X�e�[�g
	std::unique_ptr<EnemyState> state_ = nullptr;

public:
	static const float S_LENGTH_MAX_;


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon
		, IModel* model = nullptr);

private:
	//�e���m�b�N�o�b�N���ė��Ƃ�
	void KnockBack(const CollisionInfo& info);

	//��e���̃p�[�e�B�N��
	void DamageParticle(const CollisionInfo& info);

	//�^�[�Q�b�g�Ɍ������ē��������̂�
	void WalkToTarget(const Vec3& targetPos);

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon
		, IModel* model = nullptr);

	void EmergeInitialize();

	void Update() override;

	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;

	void ChangeEnemyState(std::unique_ptr<EnemyState> state);

public:
	//�ǂ�n�ʂ݂̂Ɣ���
	void CollisionWallAndFloor();
	//�ړ�
	void AllMove(const Vec3& targetPos);
	//�U��
	void Attack(const Vec3& targetPos);
	//������ύX
	void DirectionUpdate(const Vec3& targetPos);
	//hp����
	void HPUpdate();

public:
	int32_t GetWaveNum() { return waveNum_; }
	//
	int32_t GetLightIndexTmp() { return lightIndexTmp_; }
	int32_t GetLightIndexInit() { return LIGHT_INDEX_INIT_; }
};

