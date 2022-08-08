#include "Player.h"
#include "Util.h"

void Player::Attack()
{
	if (KeyboardInput::GetInstance().keyPush(DIK_SPACE) && shotTime >= shotCool)
	{
		shotTime = 0;

		//弾の速度
		const float kBulletSpeed = 1.0f;
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
	worldMat.trans.x = min(worldMat.trans.x, 63);
	worldMat.trans.x = max(worldMat.trans.x, -63);
	worldMat.trans.y = min(worldMat.trans.y, 65/2.0f);
	worldMat.trans.y = max(worldMat.trans.y, -65/2.0f);

	////回転
	//worldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_D) - KeyboardInput::GetInstance().keyPush(DIK_A)) * 0.05f;

	//行列セット
	worldMat.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle, const UINT64& bulletTexHundle)
{
	draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle);
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(view, projection, bulletTexHundle);
	}
}
