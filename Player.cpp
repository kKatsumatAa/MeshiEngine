#include "Player.h"
#include "Util.h"

//laser
Draw ray;
WorldMat rayWorld;
//ロックオンのスプライト
Draw d;
//実験用
Draw num;

Player::Player(SoundData* shotSE):
	shotSE(shotSE)
{
	//衝突属性
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
	if (KeyboardInput::GetInstance().keyTrigger(DIK_X))
	{
		if (status == NORMAL)
		{
			status = TARGET;
			isLockOn = true;
		}
		else if (status == TARGET && !KeyboardInput::GetInstance().keyPush(DIK_Z))
		{
			status = NORMAL;//仮
			isLockOn = false;
		}
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


	//射線用
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
		//////*2なのは、*1だとnear+-view.eye.zでplayerと同じ位置になっちゃうから
		//d.worldMat->SetWorld();
		//d.DrawBox(d.worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[4]);//弾の後で描画しないと透過できなくて弾が見えない！
	}
	else if (status == NORMAL)
	{
		//射線表示
		ray.DrawCube3D(&rayWorld, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[0]);
		//player
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[1]);//弾の後で描画しないと透過できなくて弾が見えない！
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
	//なにもしない
}

void Player::OnCollision2()
{
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[1], 1.0f);
}
