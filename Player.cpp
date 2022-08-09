#include "Player.h"
#include "Util.h"

Player::Player()
{
	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);
}

void Player::Attack()
{
	if (/*KeyboardInput::GetInstance().keyPush(DIK_SPACE) && */shotTime >= shotCool && status == NORMAL)
	{
		shotTime = 0;

		//�e�̑��x
		const float kBulletSpeed = 1.0f;
		Vec3 velocity(0, 0, kBulletSpeed);

		//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
		Vec3xM4(velocity, worldMat.matWorld, false);

		//���𐶐��A������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(worldMat.trans, velocity);
		//����o�^
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::Update()
{
	if (KeyboardInput::GetInstance().keyTrigger(DIK_SPACE))
	{
		if (status == NORMAL) status = TARGET;
		else if (status == TARGET) status = NORMAL;
	}

	{
		//�e������
		bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet)
			{
				return bullet->IsDead();
			}
		);

		Attack();

		for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
		{
			bullet->Update();
		}

		shotTime++;
	}


	//���ړ�
	worldMat.trans.x += (KeyboardInput::GetInstance().keyPush(DIK_D) || KeyboardInput::GetInstance().keyPush(DIK_RIGHTARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_A) || KeyboardInput::GetInstance().keyPush(DIK_LEFTARROW));
	//�c�ړ�
	worldMat.trans.y += (KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_UPARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_S) || KeyboardInput::GetInstance().keyPush(DIK_DOWNARROW));
	//�ړ�����
	worldMat.trans.x = min(worldMat.trans.x, playerMoveRange.x);
	worldMat.trans.x = max(worldMat.trans.x, -playerMoveRange.x);
	worldMat.trans.y = min(worldMat.trans.y, playerMoveRange.y);
	worldMat.trans.y = max(worldMat.trans.y, -playerMoveRange.y);

	////��]
	//worldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_D) - KeyboardInput::GetInstance().keyPush(DIK_A)) * 0.05f;

	//�s��Z�b�g
	worldMat.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}
	if (status == TARGET)
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[4]);//�e�̌�ŕ`�悵�Ȃ��Ɠ��߂ł��Ȃ��Ēe�������Ȃ��I
	else if (status == NORMAL)
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[1]);//�e�̌�ŕ`�悵�Ȃ��Ɠ��߂ł��Ȃ��Ēe�������Ȃ��I
}


Vec3 Player::GetWorldPos()
{
	Vec3 pos;
	pos.x = worldMat.matWorld.m[3][0];
	pos.y = worldMat.matWorld.m[3][1];
	pos.z = worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Player::OnCollision()
{
	//�Ȃɂ����Ȃ�
}