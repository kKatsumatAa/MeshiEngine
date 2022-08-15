#include "EnemyBullet.h"

void EnemyBullet::Initialize(const Vec3& position, const Vec3& velocity)
{
	velocity_ = velocity;

	worldMat.trans = position;

	//z‚ÉL‚Ñ‚½Œ`ó
	worldMat.scale.x = 0.5f;
	worldMat.scale.y = 0.5f;
	worldMat.scale.z = 1.0f;

	//y‚¶‚­‚Ü‚í‚è
	worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
	//y²ü‚è‚É-ƒÆ‰ñ‚·‰ñ“]s—ñ
	WorldMat worldMat2;
	worldMat2.rot = worldMat.rot;
	M4 m4;
	worldMat2.SetWorld();
	m4 = worldMat2.matWorld;
	//velocity‚É‰ñ“]s—ñ‚ğŠ|‚¯Z‚µ‚ÄvelocityZ‚ğ‹‚ß‚éi‘—¿QÆj
	Vec3 v = velocity_;
	Vec3xM4(v, m4, false);//‚±‚±‚Åã‰º‚ÌŠp“x‚ğ”½“]
	//x²ü‚èŠp“x
	worldMat.rot.x = std::atan2(-v.y, v.z);

	//Õ“Ë‘®«
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

void EnemyBullet::Update()
{
//ƒz[ƒ~ƒ“ƒO
	Vec3 toPlayer = player_->GetWorldPos() - worldMat.trans;

	toPlayer.Normalized();
	//velocity_.Normalized();

	velocity_ = SlerpVec3(velocity_, toPlayer, 0.03f);//‚±‚±‚Å‘¬“x’²®

	worldMat.trans += velocity_;

//Šp“x‚ğvector‚É‚»‚ë‚¦‚é
	//y‚¶‚­‚Ü‚í‚è
	worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
	//y²ü‚è‚É-ƒÆ‰ñ‚·‰ñ“]s—ñ
	WorldMat worldMat2;
	worldMat2.rot = worldMat.rot;
	M4 m4;
	worldMat2.SetWorld();
	m4 = worldMat2.matWorld;
	//velocity‚É‰ñ“]s—ñ‚ğŠ|‚¯Z‚µ‚ÄvelocityZ‚ğ‹‚ß‚éi‘—¿QÆj
	Vec3 v = velocity_;
	Vec3xM4(v, m4, false);//‚±‚±‚Åã‰º‚ÌŠp“x‚ğ”½“]
	//x²ü‚èŠp“x
	worldMat.rot.x = std::atan2(-v.y, v.z);

	worldMat.SetWorld();

	if (--deathTimer_ <= 0)
	{
		isDead = true;
	}
}

void EnemyBullet::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
{
	draw.DrawSphere(&worldMat, &view, &projection, { 1.0f,0,0,1.0f }, texHundle);
}

Vec3 EnemyBullet::GetWorldPos()
{
	return Vec3(worldMat.trans);
}

void EnemyBullet::OnCollision()
{
	isDead = true;
}

void EnemyBullet::OnCollision2()
{
}
