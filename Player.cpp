#include "Player.h"
#include "Util.h"

//laser
Draw ray;
WorldMat rayWorld;
Draw d;

Player::Player()
{
	//Õ“Ë‘®«
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);

	rayWorld.scale = { 0.1f,0.1f,30.0f };
	rayWorld.trans = { 0,0,150.0f };
	rayWorld.SetWorld();
}

void Player::Attack()
{
	if (/*KeyboardInput::GetInstance().keyPush(DIK_SPACE) && */shotTime >= shotCool && status == NORMAL)
	{
		shotTime = 0;

		//’e‚Ì‘¬“x
		const float kBulletSpeed = 2.0f;
		Vec3 velocity(0, 0, kBulletSpeed);

		//‘¬“xƒxƒNƒgƒ‹‚ğ©‹@‚ÌŒü‚«‚É‡‚í‚¹‚Ä‰ñ“]‚³‚¹‚é
		Vec3xM4(velocity, worldMat.matWorld, false);

		//‹…‚ğ¶¬A‰Šú‰»
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(worldMat.trans, velocity);
		//‹…‚ğ“o˜^
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
		//’e‚ğÁ‚·
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


	//‰¡ˆÚ“®
	worldMat.trans.x += (KeyboardInput::GetInstance().keyPush(DIK_D) || KeyboardInput::GetInstance().keyPush(DIK_RIGHTARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_A) || KeyboardInput::GetInstance().keyPush(DIK_LEFTARROW));
	//cˆÚ“®
	worldMat.trans.y += (KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_UPARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_S) || KeyboardInput::GetInstance().keyPush(DIK_DOWNARROW));
	//ˆÚ“®§ŒÀ
	worldMat.trans.x = min(worldMat.trans.x, playerMoveRange.x);
	worldMat.trans.x = max(worldMat.trans.x, -playerMoveRange.x);
	worldMat.trans.y = min(worldMat.trans.y, playerMoveRange.y);
	worldMat.trans.y = max(worldMat.trans.y, -playerMoveRange.y);

	////‰ñ“]
	//worldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_D) - KeyboardInput::GetInstance().keyPush(DIK_A)) * 0.05f;

	//s—ñƒZƒbƒg
	worldMat.SetWorld();


	//Ëü—p
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
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[4]);//’e‚ÌŒã‚Å•`‰æ‚µ‚È‚¢‚Æ“§‰ß‚Å‚«‚È‚­‚Ä’e‚ªŒ©‚¦‚È‚¢I
	else if (status == NORMAL)
	{
		//Ëü•\¦
		ray.DrawCube3D(&rayWorld, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[0]);
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[1]);//’e‚ÌŒã‚Å•`‰æ‚µ‚È‚¢‚Æ“§‰ß‚Å‚«‚È‚­‚Ä’e‚ªŒ©‚¦‚È‚¢I
		//3d->2d
		XMFLOAT2 pos = Vec3toXMFLOAT2(ray.worldMat->trans, view.matView, projection.matProjection);
		d.DrawBoxSprite({pos.x,pos.y,0},0.1f, { 1.0, 1.0, 1.0, 1.0 },0.0f, texHundle[4]);
	}
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
	//‚È‚É‚à‚µ‚È‚¢
}