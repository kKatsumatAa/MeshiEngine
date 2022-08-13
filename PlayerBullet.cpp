#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Vec3& position, const Vec3& velocity)
{
	velocity_ = velocity;

	worldMat.trans = position;
	worldMat.scale = { scale,scale,scale };

	//Õ“Ë‘®«
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);
}

void PlayerBullet::Update()
{
	worldMat.trans += velocity_;

	worldMat.SetWorld();

	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}
}

void PlayerBullet::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
{
	draw.DrawSphere(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle);
}


Vec3 PlayerBullet::GetWorldPos()
{
	return Vec3(worldMat.trans);
}

void PlayerBullet::OnCollision()
{
	isDead_ = true;
}

void PlayerBullet::OnCollision2()
{
}
