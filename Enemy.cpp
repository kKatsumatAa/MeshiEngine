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
//	//接近フェーズ初期化
//	InitializeApproach();
//}

void Enemy::Initialize(Player* player,BulletManager* bulletManager, const Vec3& pos)
{
	player_ = player;
	this->bulletManager = bulletManager;
	this->worldMat.trans = pos;

	state = new EnemyStateApproach;
	state->SetEnemy(this);

	//接近フェーズ初期化
	//InitializeApproach();

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
	if (KeyboardInput::GetInstance().keyReleaseTrigger(DIK_Z) && isLockOn)
	{
		isLockOnDead = true;//仮
	}
	if (isLockOnDead)
	{
		lockOnLength -= 10;
		if (lockOnLength <= 0) isDead = true;
	}
	else if(isLockOn)
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
	////弾
	//for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	//{
	//	bullet->Update();
	//}


	//timerを消す
	timedCalls_.remove_if([](std::unique_ptr<TimedCall>& time)
		{
			return time->IsFinished();
		}
	);
	////弾を消す
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
	//弾の速度
	const float kBulletSpeed = -1.0f;

	Vec3 length = player_->GetWorldPos() - GetWorldPos();
	length.Normalized();

	Vec3 velocity(0.8f, 0, 0/*length*/);//ここで発射時の角度,位置を決める

	//速度ベクトルを自機の向きに合わせて回転させる
	Vec3xM4(velocity, worldMat.matWorld, false);

	//球を生成、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
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
	draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[3]);
	if (isLockOn)
	{
		if (isLockOnScale >= 0.1f) isLockOnScale -= 0.06f;
		//lockonの線
		lockOnLine.DrawLine(worldMat.trans, worldMat.trans + lockOnVec * lockOnLength, lockOnLine.worldMat,
			&view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle[0]);

		//lockonのマーク
		Vec2 v = Vec3toVec2(worldMat.trans, view.matView, projection.matProjection);
		lockOn.DrawBoxSprite({ v.x,v.y,0 }, isLockOnScale, { 1.0f,0.0f,0.0f,1.0f }, texHundle[4], { 0.5f,0.5f });
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
	pos.x = worldMat.matWorld.m[3][0];
	pos.y = worldMat.matWorld.m[3][1];
	pos.z = worldMat.matWorld.m[3][2];

	return Vec3(pos);
}

void Enemy::OnCollision()
{
	isDead = true;
}

void Enemy::OnCollision2()
{
	isLockOn = true;
}

//----------------------------------------------
void EnemyStateApproach::Update()
{
	//移動（ベクトルを加算）
	enemy->MoveTrans(approachSpeed);
	//既定の位置に達したら離脱
	if (enemy->GetTrans().z < 10.0f)
	{
		enemy->RemoveTimeCall();
		enemy->ChangeState(new EnemyStateLeave);
	}
}

//----------------------------------------------
void EnemyStateLeave::Update()
{
	//移動
	enemy->MoveTrans(leaveSpeed);
	Vec3 v = enemy->GetWorldPos();
	if(v.x >= playerMoveRange.x + 50.0f || v.x <= -playerMoveRange.x - 50.0f||
		v.y >= playerMoveRange.y + 50.0f || v.y <= -playerMoveRange.y - 50.0f)
	enemy->SetIsDead(true);
}

//----------------------------------------------
void EnemyState::SetEnemy(Enemy* enemy)
{
	this->enemy = enemy;
}
