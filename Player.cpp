#include "Player.h"
#include "Util.h"

//laser
Draw ray;
WorldMat rayWorld;

Player::Player()
{
	//衝突属性
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);
}

void Player::Attack()
{
	if (/*KeyboardInput::GetInstance().keyPush(DIK_SPACE) && */shotTime >= shotCool && status == NORMAL)
	{
		shotTime = 0;

		//弾の速度
		const float kBulletSpeed = 2.0f;
		Vec3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		Vec3xM4(velocity, worldMat.matWorld, false);

		//球を生成、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(worldMat.trans, velocity);
		//球を登録
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
		//弾を消す
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


	//横移動
	worldMat.trans.x += (KeyboardInput::GetInstance().keyPush(DIK_D) || KeyboardInput::GetInstance().keyPush(DIK_RIGHTARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_A) || KeyboardInput::GetInstance().keyPush(DIK_LEFTARROW));
	//縦移動
	worldMat.trans.y += (KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_UPARROW)) -
		(KeyboardInput::GetInstance().keyPush(DIK_S) || KeyboardInput::GetInstance().keyPush(DIK_DOWNARROW));
	//移動制限
	worldMat.trans.x = min(worldMat.trans.x, playerMoveRange.x);
	worldMat.trans.x = max(worldMat.trans.x, -playerMoveRange.x);
	worldMat.trans.y = min(worldMat.trans.y, playerMoveRange.y);
	worldMat.trans.y = max(worldMat.trans.y, -playerMoveRange.y);

	////回転
	//worldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_D) - KeyboardInput::GetInstance().keyPush(DIK_A)) * 0.05f;

	//行列セット
	worldMat.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}
	if (status == TARGET)
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[4]);//弾の後で描画しないと透過できなくて弾が見えない！
	else if (status == NORMAL)
	{
		//ray
		ray.DrawLine({ worldMat.trans.x,worldMat.trans.y,worldMat.trans.z }, { worldMat.trans.x,worldMat.trans.y,worldMat.trans.z + 500.0f },
			&rayWorld, &view, &projection, { 1.0f,1.0f,0.0f,1.0f }, texHundle[0]);
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[1]);//弾の後で描画しないと透過できなくて弾が見えない！
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
	//なにもしない
}