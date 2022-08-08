#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Vec3& position, const Vec3& velocity)
{
	velocity_ = velocity;

	worldMat.trans = position;
	worldMat.scale = { scale,scale,scale };
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
	draw.DrawCube3D(&worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f }, texHundle);
}
