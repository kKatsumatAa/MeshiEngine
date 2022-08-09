#include "Enemy.h"

//Enemy::Enemy(Player* player)
//{
//	player_ = player;
//
//	worldMat.trans = { 0,5,100 };
//
//	state = new EnemyStateApproach;
//	state->SetEnemy(this);
//
//	//�ڋ߃t�F�[�Y������
//	InitializeApproach();
//}

void Enemy::Initialize(Player* player, const Vec3& pos)
{
	player_ = player;
	this->worldMat.trans = pos;

	state = new EnemyStateApproach;
	state->SetEnemy(this);

	//�ڋ߃t�F�[�Y������
	InitializeApproach();
}

Enemy::~Enemy()
{
	delete state;
}

void Enemy::InitializeApproach()
{
	//���˃^�C�}�[��������
	//shotTime = shotCool;

	ShotResetTimer();
}

void Enemy::Update()
{
	//timer
	for (std::unique_ptr<TimedCall>& time : timedCalls_)
	{
		time->Update();
	}
	//�e
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}


	//timer������
	timedCalls_.remove_if([](std::unique_ptr<TimedCall>& time)
		{
			return time->IsFinished();
		}
	);
	//�e������
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet)
		{
			return bullet->IsDead();
		}
	);

	state->Update();

	worldMat.SetWorld();


}

void Enemy::Fire()
{
	//�e�̑��x
	const float kBulletSpeed = -1.0f;

	Vec3 length = player_->GetWorldPos() - GetWorldPos();
	length.Normalized();

	Vec3 velocity(0.8f, 0, 0/*length*/);//�����Ŕ��ˎ��̊p�x,�ʒu�����߂�

	//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
	Vec3xM4(velocity, worldMat.matWorld, false);

	//���𐶐��A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(worldMat.trans, velocity);
	newBullet->SetPlayer(player_);
	//����o�^
	bullets_.push_back(std::move(newBullet));

	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

void Enemy::ChangeState(EnemyState* state)
{
	delete this->state;
	this->state = state;
	state->SetEnemy(this);
}

void Enemy::ShotResetTimer()
{
	//����
	Fire();

	//���˃^�C�}�[���Z�b�g
	timedCalls_.push_back(std::make_unique<TimedCall>
		(std::bind(&Enemy::ShotResetTimer, this), shotCool));
	//���R���X�g���N�^���N����
	//TimedCall::TimedCall(std::function<void()> f, uint32_t time)
	//�@.shotresetTimer��function�ɓ����
	//�A.TimedCall�̃C���X�^���X���쐬
	//�B.��������X�g�ɒǉ�(push_buck)
}

void Enemy::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[3]);

	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}
}

Vec3 Enemy::GetTrans()
{
	return Vec3(worldMat.trans);
}

void Enemy::MoveTrans(const Vec3& vec)
{
	worldMat.trans += vec;
}

void Enemy::RemoveTimeCall()
{
	timedCalls_.remove_if
	([](std::unique_ptr<TimedCall>& time) {return true; });
}

Vec3 Enemy::GetWorldPos()
{
	Vec3 pos;
	pos.x = worldMat.matWorld.m[3][0];
	pos.y = worldMat.matWorld.m[3][1];
	pos.z = worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Enemy::OnCollision()
{
	//�Ȃɂ����Ȃ�
}


//----------------------------------------------
void EnemyStateApproach::Update()
{
	//�ړ��i�x�N�g�������Z�j
	enemy->MoveTrans(approachSpeed);
	//����̈ʒu�ɒB�����痣�E
	if (enemy->GetTrans().z < 10.0f)
	{
		enemy->RemoveTimeCall();
		enemy->ChangeState(new EnemyStateLeave);
	}
}

//----------------------------------------------
void EnemyStateLeave::Update()
{
	//�ړ�
	enemy->MoveTrans(leaveSpeed);
}

//----------------------------------------------
void EnemyState::SetEnemy(Enemy* enemy)
{
	this->enemy = enemy;
}
