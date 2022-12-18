#pragma once
#include"Vec3.h"
#include"Vec2.h"
#include"CollisionConfig.h"
#include"Model.h"
#include"KeyboardInput.h"
#include"DebugText.h"
#include"Util.h"
#include "Assert.h"
#include "WindowsApp.h"
#include <random>
#include "Sound.h"

//��
	//�����V�[�h������
static std::random_device seed_gen;
//�����Z���k�c�C�X�^�[
static std::mt19937_64 engine(seed_gen());
//�����͈�
static std::uniform_real_distribution<float> posDistX(-5.0, 5.0f);
static std::uniform_real_distribution<float> posDistY(-10.0f, 10.0f);

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
	float radius_ = 1.0f;
	//���S�t���O
	bool isDead = false;

	WorldMat worldTransform_;

	//�v���[���[���ǂ����i���莞�Ɏg���j
	bool isPlayer = false;

	int HP = 0;

	//�G�̐Ԃ������i�_���[�W�󂯂镔���j
	float enemyRedRate = 0;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	int GetHP() { return HP; }
	void SetHP(int hp) { HP = hp; }
	float GetRadius() { return radius_; };
	bool GetIsPlayer() { return isPlayer; }
	void SetRadius(const float& radius) { this->radius_ = radius; };
	float GetEnemyRedRate() { return enemyRedRate; }
	void SetEnemyRedRate(float enemyRedRate) { this->enemyRedRate = enemyRedRate; }

	//�Փˎ��ɌĂ΂��
	virtual void OnCollision(Collider& collider) = 0;
	//��ƓG�̔���p
	virtual void OnCollision2(Collider& collider) = 0;

	void SetWorldPos(Vec3 pos) { worldTransform_.trans = pos; }//
	Vec3 GetWorldPos() { return worldTransform_.trans; }
	WorldMat* GetWorldTransForm() { return &worldTransform_; }

	//�Փ˗p�r�b�g�n
	//����
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(const uint32_t& bit);
	//�G
	uint32_t GetCollisionMask();
	void SetCollisionMask(const uint32_t& bit);
};

