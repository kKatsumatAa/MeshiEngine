#include "Player.h"
#include "Util.h"

//laser
Draw ray;
WorldMat rayWorld;
//���b�N�I���̃X�v���C�g
Draw d;
//�����p
Draw num;

Player::Player(SoundData* shotSE):
	shotSE(shotSE)
{
	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);

	rayWorld.scale = { 0.1f,0.1f,30.0f };
	rayWorld.trans = { 0,0,150.0f };
	rayWorld.SetWorld();
}

void Player::Attack()
{
	if (KeyboardInput::GetInstance().keyPush(DIK_Z) && shotTime >= shotCool && status == NORMAL)
	{
		SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[0], 1.0f);

		shotTime = 0;

		//�e�̑��x
		const float kBulletSpeed = 2.0f;
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
	if (KeyboardInput::GetInstance().keyTrigger(DIK_X))
	{
		if (status == NORMAL)
		{
			status = TARGET;
			isLockOn = true;
		}
		else if (status == TARGET && !KeyboardInput::GetInstance().keyPush(DIK_Z))
		{
			status = NORMAL;//��
			isLockOn = false;
		}
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


	//�ː��p
	rayWorld.trans = { worldMat.trans.x,worldMat.trans.y,rayWorld.trans.z };
	rayWorld.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}
	if (status == TARGET)
	{
		//3d->2d
		Vec2 pos = Vec3toVec2({ worldMat.trans.x,worldMat.trans.y,worldMat.trans.z /*- view.eye.z*/ }, view.matView, projection.matProjection);
		d.DrawBoxSprite({ pos.x,pos.y,0 }, 0.2f, { 1.0, 1.0, 1.0, 1.0 }, texHundle[4], { 0.5f,0.5f });
		//2d->3d
		//Vec2 pos = Vec3toVec2({ worldMat.trans.x,worldMat.trans.y,worldMat.trans.z - view.eye.z }, view.matView, projection.matProjection);
		//d.worldMat->trans = Vec2toVec3(pos, view.matView, projection.matProjection, worldMat.trans.z - view.eye.z * 2);
		//////*2�Ȃ̂́A*1����near+-view.eye.z��player�Ɠ����ʒu�ɂȂ����Ⴄ����
		//d.worldMat->SetWorld();
		//d.DrawBox(d.worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[4]);//�e�̌�ŕ`�悵�Ȃ��Ɠ��߂ł��Ȃ��Ēe�������Ȃ��I
	}
	else if (status == NORMAL)
	{
		//�ː��\��
		ray.DrawCube3D(&rayWorld, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[0]);
		//player
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[1]);//�e�̌�ŕ`�悵�Ȃ��Ɠ��߂ł��Ȃ��Ēe�������Ȃ��I
	}
	num.DrawClippingBoxSprite({ 0,0,0 }, 1.0f, { 0,0 }, { 0.1f * isLockNum,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, 0.0f, texHundle[0]);
}

Vec3 Player::GetWorldPos()
{
	Vec3 pos;
	pos.x = (float)worldMat.matWorld.m[3][0];
	pos.y = (float)worldMat.matWorld.m[3][1];
	pos.z = (float)worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Player::OnCollision()
{
	//�Ȃɂ����Ȃ�
}

void Player::OnCollision2()
{
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[1], 1.0f);
}
