#include "EnemyBullet.h"

void EnemyBullet::Initialize(const Vec3& position, const Vec3& velocity, int BulletVariation)
{
	velocity_ = velocity;

	this->BulletVariation = BulletVariation;

	worldMat.trans = position;

	worldMat.scale.x = 0.5f;
	worldMat.scale.y = 0.5f;
	worldMat.scale.z = 0.5f;

	//�����蔻��p�̔��a������
	radius_ *= worldMat.scale.x;

	if (BulletVariation == HOMING)
	{
		//z�ɐL�т��`��
		worldMat.scale.x = 0.5f;
		worldMat.scale.y = 0.5f;
		worldMat.scale.z = 1.0f;

		//y�����܂��
		worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
		//y�������-�Ɖ񂷉�]�s��
		WorldMat worldMat2;
		worldMat2.rot = worldMat.rot;
		M4 m4;
		worldMat2.SetWorld();
		m4 = worldMat2.matWorld;
		//velocity�ɉ�]�s����|���Z����velocityZ�����߂�i�����Q�Ɓj
		Vec3 v = velocity_;
		Vec3xM4(v, m4, false);//�����ŏ㉺�̊p�x�𔽓]
		//x������p�x
		worldMat.rot.x = std::atan2(-v.y, v.z);
	}
	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);
}

void EnemyBullet::Update()
{
//�z�[�~���O
	if (BulletVariation == HOMING)
	{
		Vec3 V = player_->GetWorldPos() - worldMat.trans;
		if (V.GetLength() > 30.0f)
		{
			Vec3 toPlayer = player_->GetWorldPos() - worldMat.trans;

			toPlayer.Normalized();
			//velocity_.Normalized();

			velocity_ = SlerpVec3(velocity_, toPlayer, 0.03f);//�����ő��x����
		}
	}
	worldMat.trans += velocity_;
//�p�x��vector�ɂ��낦��
	if (BulletVariation == HOMING)
	{
		//y�����܂��
		worldMat.rot.y = std::atan2(velocity_.x, velocity_.z);
		//y�������-�Ɖ񂷉�]�s��
		WorldMat worldMat2;
		worldMat2.rot = worldMat.rot;
		M4 m4;
		worldMat2.SetWorld();
		m4 = worldMat2.matWorld;
		//velocity�ɉ�]�s����|���Z����velocityZ�����߂�i�����Q�Ɓj
		Vec3 v = velocity_;
		Vec3xM4(v, m4, false);//�����ŏ㉺�̊p�x�𔽓]
		//x������p�x
		worldMat.rot.x = std::atan2(-v.y, v.z);
	}

	worldMat.SetWorld();

	if (--deathTimer_ <= 0)
	{
		isDead = true;
	}
}

void EnemyBullet::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
{
	if (BulletVariation == HOMING)
		draw.DrawSphere(&worldMat, &view, &projection, { 1.0f,0,1.0f,1.0f }, texHundle);
	if (BulletVariation == NORM)
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
