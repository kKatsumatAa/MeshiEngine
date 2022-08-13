#pragma once
#include "Assert.h"
#include"TimedCall.h"
#include"EnemyBullet.h"
#include"BulletManager.h"


//���@�N���X�̑O���錾(�C���N���[�h��������)
class Player;

class Enemy;

class EnemyState
{
protected:
	Enemy* enemy = nullptr;

public:
	void SetEnemy(Enemy* enemy);
	virtual void Update() = 0;
};

///<summary>
///�G
///</summary>
class Enemy : public Collider
{
	//�ϐ�
private:
	//���[���h�ϊ��f�[�^
	WorldMat worldMat;
	//���f��
	Draw draw;
	Draw lockOn;
	//��ԁi�s���j
	EnemyState* state;
	////�e
	//std::list< std::unique_ptr<EnemyBullet>> bullets_;
	static const int shotCool = 60;
	std::list<std::unique_ptr<TimedCall>> timedCalls_;

	//���L����
	Player* player_ = nullptr;

	//�e�}�l�[�W���[
	BulletManager* bulletManager = nullptr;

	bool isDead = false;
	bool isLockOn = false;

public:
	//int shotTime = 0;


	//�֐�
private:


public:
	/*Enemy(Player* player);*/
	void Initialize(Player* player, BulletManager* bulletManager,const Vec3& pos);
	~Enemy();
	void InitializeApproach();

	void Update();
	void Fire();
	void ChangeState(EnemyState* state);
	/// <summary>
	/// �e���ˁA�^�C�}�[���Z�b�g
	/// </summary>
	/// <param name="view"></param>
	void ShotResetTimer();
	void Draw(ViewMat& view,ProjectionMat& projection, const UINT64* texHundle);

	Vec3 GetTrans();
	void MoveTrans(const Vec3& vec);

	void RemoveTimeCall();

	void SetPlayer(Player* player) { player_ = player; }

	Vec3 GetWorldPos() override;

	void SetIsDead(const bool isDead) { this->isDead = isDead; }

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision()override;
	/// <summary>
	/// player�Ƀ��b�N�I�����ꂽ�Ƃ��p
	/// </summary>
	void OnCollision2()override;

	//�e���X�g���擾(const�Q��)
	//const std::list<std::unique_ptr<EnemyBullet>>& GetBullets()
	//{
	//	return bullets_;
	//}

	bool IsDead() { return isDead; }
};

class EnemyStateApproach :public EnemyState
{
public:
	void Update();
};

class EnemyStateLeave :public EnemyState
{
public:
	void Update();
};


const Vec3 approachSpeed = { 0,0,-0.2 };
const Vec3 leaveSpeed = { -0.2,0.2,0 };