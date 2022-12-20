#include "PlayerBullet.h"
#include <math.h>

void PlayerBullet::Initialize(const Vec3& position, const Vec3& velocity, Model* model)
{
	assert(model);

	worldTransform_.trans = position;
	this->velocity = velocity;
	this->velocityTmp = velocity;
	this->model = model;

	radius_ = scaleTmp.x;
	worldTransform_.scale = { radius_,radius_ ,radius_ };

	worldTransform_.SetWorld();


	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);
}

void PlayerBullet::Update()
{
	count++;

	//�ړ���������
	velocity = LerpVec3({ 0,0,0 }, { velocityTmp * 1.5f }, ((deathTimer_) / (float)lifeTime));
	worldTransform_.scale = scaleTmp + LerpVec3({ velocityTmp }, { 0,0,0 }, ((deathTimer_) / (float)lifeTime));
	worldTransform_.SetWorld();

	//���R���ł܂ł̃^�C�}�[
	deathTimer_--;
	if (deathTimer_ <= 0)
	{
		isDead = true;
	}
}

void PlayerBullet::Draw(ViewMat& view, ProjectionMat& projection)
{
	draw.DrawModel(&worldTransform_, &view, &projection, model);
}

void PlayerBullet::OnCollision(Collider& collider)
{
	isDead = true;
}

void PlayerBullet::OnCollision2(Collider& collider)
{
}
