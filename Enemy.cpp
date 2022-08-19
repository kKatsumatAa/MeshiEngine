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

void Enemy::Initialize(Player* player,BulletManager* bulletManager, const Vec3& pos)
{
	player_ = player;
	this->bulletManager = bulletManager;
	this->worldMat.trans = pos;
	this->worldMat.SetWorld();

	state = new EnemyStateApproach;
	state->SetEnemy(this);

	HP = 1;

	//�ڋ߃t�F�[�Y������
	InitializeApproach();

	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
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
	if (HP <= 0) isDead = true;

	if (KeyboardInput::GetInstance().keyReleaseTrigger(DIK_Z) && isLockOned)
	{
		isLockOnDead = true;//��
	}
	if (isLockOnDead)
	{
		lockOnLength -= 5;
		if (lockOnLength <= 0)
		{
			HP -= isLockOned;
			LockOnedReset();
		}
	}
	else if(isLockOned)
	{
		lockOnVec = player_->GetWorldPos() - worldMat.trans;
		lockOnLength = lockOnVec.GetLength();
		lockOnVec.Normalized();
	}

	//timer
	for (std::unique_ptr<TimedCall>& time : timedCalls_)
	{
		time->Update();
	}
	////�e
	//for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	//{
	//	bullet->Update();
	//}


	//timer������
	timedCalls_.remove_if([](std::unique_ptr<TimedCall>& time)
		{
			return time->IsFinished();
		}
	);
	////�e������
	//bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet)
	//	{
	//		return bullet->IsDead();
	//	}
	//);

	state->Update();

	worldMat.SetWorld();


}

void Enemy::Fire()
{
	//�e�̑��x
	const float kBulletSpeed = -1.0f;

	Vec3 length = player_->GetWorldPos() - GetWorldPos();
	length.Normalized();

	//Vec3 velocity(0.8f, 0, 0/*length*/);//�����Ŕ��ˎ��̊p�x,�ʒu�����߂�
	Vec3 velocity(length);//�����Ŕ��ˎ��̊p�x,�ʒu�����߂�

	//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
	Vec3xM4(velocity, worldMat.matWorld, false);

	//���𐶐��A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(worldMat.trans, velocity);
	newBullet->SetPlayer(player_);
	//����o�^
	bulletManager->enemyBullets_.push_back(std::move(newBullet));
	//�����̂�Enemy�N���X�ŏ���
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
	draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,0.0f,0.0f,0.7f }, texHundle[0]);

	if (isLockOned)
	{
		if (isLockOnScale >= 0.2f)
		{
			isLockOnScale -= 0.1f;
			lockOnAngle += 5.0f;
		}
		else if(lockOnAngle != 0.0f) lockOnAngle = 0.0f;

		//lockon�̐�
		if(isLockOnDead)
		lockOnLine.DrawLine(worldMat.trans, worldMat.trans + lockOnVec * lockOnLength, lockOnLine.worldMat,
			&view, &projection, { 1.0f,1.0f,0.0f,1.0f }, texHundle[0]);
		else
			lockOnLine.DrawLine(worldMat.trans, worldMat.trans + lockOnVec * lockOnLength, lockOnLine.worldMat,
				&view, &projection, { 1.0f,1.0f,1.0f,0.5f }, texHundle[0]);

		//lockon�̃}�[�N
		Vec2 v = Vec3toVec2(worldMat.trans, view.matView, projection.matProjection);
		if (isLockOnDead)
			lockOn.DrawBoxSprite({ v.x,v.y,0 }, isLockOnScale, { 1.0f,1.0f,0.0f,0.7f }, texHundle[6], { 0.5f,0.5f }, lockOnAngle);
		else
			lockOn.DrawBoxSprite({ v.x,v.y,0 }, isLockOnScale, { 1.0f,1.0f,1.0f,0.7f }, texHundle[6], { 0.5f,0.5f }, lockOnAngle);
	}
	/*for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}*/
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
	pos.x = (float)worldMat.matWorld.m[3][0];
	pos.y = (float)worldMat.matWorld.m[3][1];
	pos.z = (float)worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Enemy::OnCollision()
{
	HP--;
}

void Enemy::OnCollision2()
{
	isLockOnScale = lockOnScaleTmp;
	lockOnVec = { 0.0f,0.0f,0.0f };
	//lockOnLength = 0.0f;
	lockOnAngle = 0.0f;
	isLockOned++;
	/*player_->isLockNum++;*/
}

void Enemy::LockOnedReset()
{
	isLockOned = 0;
	isLockOnDead = false;
	isLockOnScale = lockOnScaleTmp;
	lockOnVec = { 0.0f,0.0f,0.0f };
	lockOnLength = 0.0f;
	lockOnAngle = 0.0f;
}

//----------------------------------------------
void EnemyStateApproach::Update()
{
	//�ړ��i�x�N�g�������Z�j
	enemy->MoveTrans(approachSpeed);
	//����̈ʒu�ɒB�����痣�E
	if (enemy->GetTrans().z <= 11.0f)
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
	Vec3 v = enemy->GetWorldPos();
	if (v.y >= playerMoveRange.y + 50.0f || v.y <= -playerMoveRange.y - 50.0f)
		enemy->SetIsAnnihilation(true);
}

//----------------------------------------------
void EnemyState::SetEnemy(Enemy* enemy)
{
	this->enemy = enemy;
}
