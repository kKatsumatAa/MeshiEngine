#include "Item.h"

void Item::Initialize(Player* player, const Vec3& pos, SoundData* soundData, ParticleManager* pManager)
{
	lockOnLength = 0;
	lockOnVec = { 0,0,0 };
	isLockOnScale = lockOnScaleTmp;

	lockOnAngle = 0.0f;

	//自然消滅フラグ
	isAnnihilation = false;

	posVec2 = { 0,0 };
	moveVec = { 0,0 };

	count = 0;

	lockCool = lockCooltmp;

	isLockOnDead = false;

	isGet = false;

	rightMove = false;


	this->soundData = soundData;

	player_ = player;
	this->worldMat.trans = pos;
	this->worldMat.SetWorld();
	this->pManager = pManager;

	HP = 1;

	//衝突属性
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

Item::~Item()
{
}

void Item::Update(ViewMat& view, ProjectionMat& projection)
{
	if (isDead)
	{
		isLockOned = false;
		isLockOnDead = false;
	}

	count++;

	//アイテム振動
	if (count % 20 > 10)
		worldMat.scale = { 1.5f,1.5f,1.5f };
	else if(count % 20 < 5)
		worldMat.scale = { 1.2f,1.2f,1.2f };
	else
		worldMat.scale = { 1.0f,1.0f,1.0f };

	worldMat.SetWorld();


	bool oldIsDead = isDead;

	lockCool--;

	if (HP <= 0)
	{
		isDead = true;
	}

	if (isDead)
	{
		//アイテム破壊した瞬間
		if (!oldIsDead)
		{
			pManager->GenerateRandomParticle(worldMat.trans, 120, worldMat.scale.x * 3.5f,
				{ GetColor().x,GetColor().y,GetColor().z,1.0f }, 10);

			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[3], 2.0f);

			posVec2 = Vec3toVec2(worldMat.trans, view.matView, projection.matProjection);
			moveVec = Vec2(90.0f, 650.0f) - posVec2;
			moveVec.Normalized();
		}
		posVec2 += moveVec * 10.0f;
		if (CollisionCircleCircle({ 90.0f,650.0f,0.0f }, 10.0f, { posVec2.x,posVec2.y,0.0f }, 10.0f) && !isGet)
		{
			pManager->GenerateRandomParticle(Vec2toVec3(posVec2,view.matView,projection.matProjection,100), 80, worldMat.scale.x * 3.0f,
				{ 1.0f,1.0f,1.0f,1.0f }, 10);

			isGet = true;
			if (player_->HP < 3)
			player_->HP++;
		}
	}

	//右に移動か左か
	if (rightMove) worldMat.trans.x += 0.1f;
	else           worldMat.trans.x -= 0.1f;
	
	//一定以上行ったら自然消滅
	if (worldMat.trans.x > 100.0f || worldMat.trans.x < -100.0f) isAnnihilation = true;

	worldMat.SetWorld();

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
			//ダメージ音（死亡時以外）
			if (HP > 0)  SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[4], 1.5f);
		}
	}
	else if (isLockOned)
	{
		lockOnVec = player_->GetWorldPos() - worldMat.trans;
		lockOnLength = lockOnVec.GetLength();
		lockOnVec.Normalized();
	}
}

void Item::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	if (isDead)
		draw2.DrawBoxSprite({ posVec2.x,posVec2.y,0.0f }, 0.1f, { 1.0f,1.0f,1.0f,1.0f }, texHundle[8], { 0.5f,0.5f });
	else
		draw.DrawBox(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,0.9f }, texHundle[8]);

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
}

Vec3 Item::GetTrans()
{
	return Vec3(worldMat.trans);
}

void Item::MoveTrans(const Vec3& vec)
{
	worldMat.trans += vec;
}

Vec3 Item::GetWorldPos()
{
	Vec3 pos;
	pos.x = (float)worldMat.matWorld.m[3][0];
	pos.y = (float)worldMat.matWorld.m[3][1];
	pos.z = (float)worldMat.matWorld.m[3][2];

	return Vec3(pos);
}


void Item::OnCollision()
{
	HP--;
	//ダメージ音(死亡時以外)
	if (HP > 0)  SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[4], 1.5f);
}

void Item::OnCollision2()
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

void Item::LockOnedReset()
{
	isLockOned = 0;
	isLockOnDead = false;
	isLockOnScale = lockOnScaleTmp;
	lockOnVec = { 0.0f,0.0f,0.0f };
	lockOnLength = 0.0f;
	lockOnAngle = 0.0f;
}

XMFLOAT4 Item::GetColor()
{
	return draw.GetColor();
}
