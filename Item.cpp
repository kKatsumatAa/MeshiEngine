#include "Item.h"

void Item::Initialize(const Vec3& position, const int itemType, Model* model)
{
	assert(model);

	worldTransform_.trans = position;
	this->velocity = velocity;
	this->model = model;
	this->itemType = itemType;

	radius_ = scaleTmp.x;
	worldTransform_.scale = { radius_,radius_ ,radius_ };

	worldTransform_.SetWorld();


	////è’ìÀëÆê´
	//SetCollisionAttribute(kCollisionAttributePlayer);
	//SetCollisionMask(kCollisionAttributeEnemy);
}

void Item::Update()
{
}

void Item::Draw(ViewMat& view, ProjectionMat& projection)
{
	//draw.DrawModel(&worldTransform_, &view, &projection, model);
	//âº
	XMFLOAT4 color;
	if (itemType == INORMAL)
	{
		color = { 1.0f,1.0f,0,1.0f };
	}
	else if (itemType == ILASER)
	{
		color = { 1.0f,0.0f,1.0f,1.0f };
	}
	else if (itemType == ISHOTGUN)
	{
		color = { 1.0f,1.0f,1.0f,1.0f };
	}
	draw.DrawCube3D(&worldTransform_, &view, &projection, color, TextureManager::whiteTexHandle);
}

void Item::OnCollision(Collider& collider)
{
	isDead = true;
}

void Item::OnCollision2(Collider& collider)
{
}
