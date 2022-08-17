#include "EnemyBullet.h"

void EnemyBullet::Initialize(const Vec3& position, const Vec3& velocity)
{
	velocity_ = velocity;

	worldMat.trans = position;

	worldMat.scale.x = 0.5f;
	worldMat.scale.y = 0.5f;
	worldMat.scale.z = 0.5f;

	//当たり判定用の半径小さく
	radius_ *= worldMat.scale.x;

	////zに伸びた形状
	//worldMat.scale.x = 0.5f;
	//worldMat.scale.y = 0.5f;
	//worldMat.scale.z = 1.0f;

	////yじくまわり
	//worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
	////y軸周りに-θ回す回転行列
	//WorldMat worldMat2;
	//worldMat2.rot = worldMat.rot;
	//M4 m4;
	//worldMat2.SetWorld();
	//m4 = worldMat2.matWorld;
	////velocityに回転行列を掛け算してvelocityZを求める（資料参照）
	//Vec3 v = velocity_;
	//Vec3xM4(v, m4, false);//ここで上下の角度を反転
	////x軸周り角度
	//worldMat.rot.x = std::atan2(-v.y, v.z);

	//衝突属性
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

void EnemyBullet::Update()
{
//ホーミング
	//Vec3 V = player_->GetWorldPos() - worldMat.trans;
	//if (V.GetLength() > 30.0f)
	//{
	//	Vec3 toPlayer = player_->GetWorldPos() - worldMat.trans;

	//	toPlayer.Normalized();
	//	//velocity_.Normalized();

	//	velocity_ = SlerpVec3(velocity_, toPlayer, 0.03f);//ここで速度調整
	//}
	worldMat.trans += velocity_;
//角度をvectorにそろえる
	////yじくまわり
	//worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
	////y軸周りに-θ回す回転行列
	//WorldMat worldMat2;
	//worldMat2.rot = worldMat.rot;
	//M4 m4;
	//worldMat2.SetWorld();
	//m4 = worldMat2.matWorld;
	////velocityに回転行列を掛け算してvelocityZを求める（資料参照）
	//Vec3 v = velocity_;
	//Vec3xM4(v, m4, false);//ここで上下の角度を反転
	////x軸周り角度
	//worldMat.rot.x = std::atan2(-v.y, v.z);

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
