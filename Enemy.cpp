#include "Enemy.h"

Draw HPBox;
Draw HPBox2;

//Enemy::Enemy(Player* player)
//{
//	player_ = player;
//
//	worldMat.trans = { 0,5,100 };
//
//	state = new EnemyStateApproach;
//	state->SetEnemy(this);
//
//	//接近フェーズ初期化
//	InitializeApproach();
//}

void Enemy::Initialize(Player* player,BulletManager* bulletManager, const Vec3& pos, SoundData* soundData)
{
	this->soundData = soundData;

	player_ = player;
	this->bulletManager = bulletManager;
	this->worldMat.trans = pos;
	this->worldMat.SetWorld();

	state = new EnemyStateApproach;
	state->SetEnemy(this);

	HP = 1;

	//接近フェーズ初期化
	InitializeApproach();

	//衝突属性
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

Enemy::~Enemy()
{
	delete state;
}

void Enemy::InitializeApproach()
{
	//発射タイマーを初期化
	//shotTime = shotCool;

	ShotResetTimer();
}

void Enemy::Update()
{
	if (isBoss)
	{
		worldMat.rot.z += 0.07f;
		worldMat.SetWorld();

		if (worldMat.scale.z > bossScale)
		{
			worldMat.scale -= {0.2f, 0.2f, 0.2f};
			worldMat.SetWorld();
		}
	}


	lockCool--;

	if (HP <= 0) isDead = true;

	if (KeyboardInput::GetInstance().keyReleaseTrigger(DIK_Z) && isLockOned)
	{
		isLockOnDead = true;//仮
	}
	if (isLockOnDead)
	{
		lockOnLength -= 7.5f;
		if (lockOnLength <= 0)
		{
			HP -= isLockOned;
			LockOnedReset();
			//ダメージ音
			if (HP > 0)  SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[4], 1.5f);
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

	//timerを消す
	timedCalls_.remove_if([](std::unique_ptr<TimedCall>& time)
		{
			return time->IsFinished();
		}
	);

	state->Update();

	worldMat.SetWorld();


}

void Enemy::Fire()
{
	//弾の速度
	const float kBulletSpeed = -1.0f;

	Vec3 length = player_->GetWorldPos() - GetWorldPos();
	length.Normalized();

	//ボスだったら横に弾発射
	Vec3 velocity;
	if (isBoss)
	{
		switch (count % 4)
		{
		case 0:
			velocity = { -0.8f, 0, 0 };
			break;
		case 1:
			velocity = { 0, 0.8f, 0 };
			break;
		case 2:
			velocity = { 0.8f, 0, 0 };
			break;
		case 3:
			velocity = { 0, -0.8f, 0 };
			break;
		}

		count++;
	}
	else if (isEnemy2)
		velocity = { -0.8f, 0, 0 };
	else
		velocity = { length };//ここで発射時の角度,位置を決める

	//速度ベクトルを自機の向きに合わせて回転させる
	Vec3xM4(velocity, worldMat.matWorld, false);

	//球を生成、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	
	if (isBoss || isEnemy2)
		newBullet->Initialize(worldMat.trans, velocity,HOMING);
	else
		newBullet->Initialize(worldMat.trans, velocity);

	newBullet->SetPlayer(player_);
	//球を登録
	bulletManager->enemyBullets_.push_back(std::move(newBullet));
	//生成のみEnemyクラスで処理
}

void Enemy::ChangeState(EnemyState* state)
{
	delete this->state;
	this->state = state;
	state->SetEnemy(this);
}

void Enemy::ShotResetTimer()
{
	//発射
	Fire();

	//発射タイマーリセット
	if (isBoss)
		timedCalls_.push_back(std::make_unique<TimedCall>
			(std::bind(&Enemy::ShotResetTimer, this), shotCool / 4.0f));
	else if(isEnemy2)
		timedCalls_.push_back(std::make_unique<TimedCall>
			(std::bind(&Enemy::ShotResetTimer, this), shotCool * 1.5f));
	else
		timedCalls_.push_back(std::make_unique<TimedCall>
			(std::bind(&Enemy::ShotResetTimer, this), shotCool));
	//↑コンストラクタも起動↓
	//TimedCall::TimedCall(std::function<void()> f, uint32_t time)
	//①.shotresetTimerをfunctionに入れる
	//②.TimedCallのインスタンスを作成
	//③.それをリストに追加(push_buck)
}

void Enemy::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	if (isBoss)
		draw.DrawCube3D(&worldMat, &view, &projection, { 0.7f,0.7f,0.0f,0.8f }, texHundle[0]);
	else if (isEnemy2)
		draw.DrawCube3D(&worldMat, &view, &projection, { 0.0f,0.9f,1.0f,0.8f }, texHundle[0]);
	else
		draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,0.0f,0.0f,0.7f }, texHundle[0]);

	if (isLockOned)
	{
		if (isLockOnScale >= 0.2f)
		{
			isLockOnScale -= 0.1f;
			lockOnAngle += 5.0f;
		}
		else if (lockOnAngle != 0.0f) lockOnAngle = 0.0f;

		//lockonの線
		if (isLockOnDead)
			lockOnLine.DrawLine(worldMat.trans, worldMat.trans + lockOnVec * lockOnLength, lockOnLine.worldMat,
				&view, &projection, { 1.0f,1.0f,0.0f,1.0f }, texHundle[0]);
		else
			lockOnLine.DrawLine(worldMat.trans, worldMat.trans + lockOnVec * lockOnLength, lockOnLine.worldMat,
				&view, &projection, { 1.0f,1.0f,1.0f,0.5f }, texHundle[0]);

		//lockonのマーク
		Vec2 v = Vec3toVec2(worldMat.trans, view.matView, projection.matProjection);
		if (isLockOnDead)
			lockOn.DrawBoxSprite({ v.x,v.y,0 }, isLockOnScale, { 1.0f,1.0f,0.0f,0.7f }, texHundle[6], { 0.5f,0.5f }, lockOnAngle);
		else
			lockOn.DrawBoxSprite({ v.x,v.y,0 }, isLockOnScale, { 1.0f,1.0f,1.0f,0.7f }, texHundle[6], { 0.5f,0.5f }, lockOnAngle);
	}
	//
	if (isBoss)
	{
		HPBox2.DrawClippingBoxSprite({ 300,40,0 }, 0.1f, { 0,0.20f }, { 0.1f * 30 * 10 + 0.05f,0.6f }, { 1.0f,1.0f,1.0f,0.6f }, 0.0f, texHundle[0]);
		HPBox.DrawClippingBoxSprite({ 300,40,0 }, 0.1f, { 0.05f,0.25f }, { 0.1f * HP * 10 - 0.05f,0.5f }, { 1.0f,0.0f,0.0f,1.0f }, 0.0f, texHundle[0]);
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
	pos.x = (float)worldMat.matWorld.m[3][0];
	pos.y = (float)worldMat.matWorld.m[3][1];
	pos.z = (float)worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Enemy::OnCollision()
{
	HP--;
	//ダメージ音
	if (HP > 0)  SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[4], 1.5f);
}

void Enemy::OnCollision2()
{
	if (lockCool <= 0)
	{
		isLockOnScale = lockOnScaleTmp;
		lockOnVec = { 0.0f,0.0f,0.0f };
		//lockOnLength = 0.0f;
		lockOnAngle = 0.0f;
		isLockOned++;
		lockCool = lockCooltmp;
		/*player_->isLockNum++;*/
	}
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
	if (!enemy->isBoss)
	{
		//移動（ベクトルを加算）
		enemy->MoveTrans(approachSpeed);
		//既定の位置に達したら離脱
		if (enemy->GetTrans().z <= 11.0f)
		{
			enemy->RemoveTimeCall();
			enemy->ChangeState(new EnemyStateLeave);
		}
	}
}

//----------------------------------------------
void EnemyStateLeave::Update()
{
	if (!enemy->isBoss)
	{
		//移動
		enemy->MoveTrans(leaveSpeed);
		Vec3 v = enemy->GetWorldPos();
		if (v.y >= playerMoveRange.y + 50.0f || v.y <= -playerMoveRange.y - 50.0f)
			enemy->SetIsAnnihilation(true);
	}
}

//----------------------------------------------
void EnemyState::SetEnemy(Enemy* enemy)
{
	this->enemy = enemy;
}
