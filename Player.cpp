#include "Player.h"
#include "Util.h"

//laser
Draw ray;
WorldMat rayWorld;
//ロックオンのスプライト
Draw d;
//実験用
Draw num;
Draw hp[3];

void Player::Initialize(SoundData* shotSE)
{
	this->shotSE = shotSE;
	bullets_.clear();

	shotTime = 0;
	status = NORMAL;
	coolColor = 1.0f;
	//継承してる系はplayerはリスト管理ではないので自分で初期化しないとダメ！！！！
	 coolTime = 0;
	 HP = 3;
	 isDead = false;
	 isLockOn = false;
	 isLockNum = 0;

	 worldMat.trans = { 0,0,0 };
	 worldMat.SetWorld();

	rayWorld.scale = { 0.1f,0.1f,30.0f };
	rayWorld.trans = { 0,0,150.0f };
	rayWorld.SetWorld();
}

Player::Player()
{
	//衝突属性
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);

	rayWorld.scale = { 0.1f,0.1f,30.0f };
	rayWorld.trans = { 0,0,150.0f };
	rayWorld.SetWorld();

	HP = 3;
}

void Player::Attack()
{
	if (KeyboardInput::GetInstance().keyPush(DIK_Z) && shotTime >= shotCool && status == NORMAL)
	{
		SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[0], 0.2f);

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
	if (HP <= 0) isDead = true;
	coolTime--;

	if (KeyboardInput::GetInstance().keyTrigger(DIK_R))
	{
		HP = 3;
		isDead = false;
		coolTime = 0;
	}

	if (!isDead)
	{
		//playerクラス内での処理に使うフラグ
		if (KeyboardInput::GetInstance().keyTrigger(DIK_X))
		{
			if (status == NORMAL)
			{
				status = TARGET;
				//isLockOn = true;
			}
			else if (status == TARGET)
			{
				status = NORMAL;//仮
				//isLockOn = false;
			}
		}

		//当たり判定の時に使うロックオン状態かどうかのフラグ処理
		if (status == TARGET && KeyboardInput::GetInstance().keyPush(DIK_Z)) isLockOn = true;
		else isLockOn = false;


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

		//弾
		Attack();
		shotTime++;
	}

	//弾の更新
	{
		//弾を消す
		bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet)
			{
				return bullet->IsDead();
			}
		);

		for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
		{
			bullet->Update();
		}	
	}
}

void Player::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle, const UINT64* numTexHundle)
{
	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, texHundle[0]);
	}

	//無敵時間の時は色を薄く
	if (coolTime > 0)
	{
		//点滅
		if (coolTime % 20 > 10)
			coolColor = 0.0f;
		else
			coolColor = 1.0f;
	}
	else coolColor = 1.0f;

	if (!isDead)
	{
		if (status == TARGET)
		{
			//3d->2d
			Vec2 pos = Vec3toVec2({ worldMat.trans.x,worldMat.trans.y,worldMat.trans.z /*- view.eye.z*/ }, view.matView, projection.matProjection);
			d.DrawBoxSprite({ pos.x,pos.y,0 }, 0.25f, { 0.8f, 0.8f, 0.8f, 0.8f * coolColor }, texHundle[4], { 0.5f,0.5f });

			//ロックオンの数
			if (isLockNum == 10)
				num.DrawBoxSprite({ pos.x,pos.y,0 }, 0.16f, { 1.0f, 0.8f, 0.8f, 0.9f }, numTexHundle[10], { 0.5f,0.5f });
			else
				num.DrawBoxSprite({ pos.x,pos.y,0 }, 0.08f, { 0.8f, 0.8f, 0.8f, 0.8f }, numTexHundle[isLockNum], { 0.5f,0.5f });
		}
		else if (status == NORMAL)
		{
			//射線表示
			ray.DrawCube3D(&rayWorld, &view, &projection, { 1.0f,1.0f,1.0f,0.7f }, texHundle[0]);
			//player
			draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.7f * coolColor }, texHundle[0]);//弾の後で描画しないと透過できなくて弾が見えない！
		}

		//hpとかロックオンの数
		if (HP > 0)
		{
			if (HP >= 1)
				hp[0].DrawBoxSprite({ 50.0f,650.0f,0 }, 0.1f, { 1.0f,1.0f,1.0f,0.6f }, texHundle[8]);
			if (HP >= 2)
				hp[1].DrawBoxSprite({ 90.0f,650.0f,0 }, 0.1f, { 1.0f, 1.0f,1.0f,0.6f }, texHundle[8]);
			if (HP >= 3)
				hp[2].DrawBoxSprite({ 130.0f,650.0f,0 }, 0.1f, { 1.0f,1.0f,1.0f,0.6f }, texHundle[8]);
		}
	}
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
	if (coolTime <= 0)
	{
		HP--;
		coolTime = coolTimetmp;
		SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[5], 1.0f);
	}
}

void Player::OnCollision2()
{
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), shotSE[1], 1.0f);
}
