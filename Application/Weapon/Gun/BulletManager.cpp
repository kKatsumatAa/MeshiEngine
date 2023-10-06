#include "BulletManager.h"

BulletManager& BulletManager::GetInstance()
{
	static BulletManager inst;
	return inst;
}

void BulletManager::Initialize()
{
	bullets_.clear();
}

void BulletManager::CreateBullet(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner)
{
	std::unique_ptr<Bullet>bullet = Bullet::Create(pos, directionVec, scale, lifeTime, owner);
	bullets_.push_back(std::move(bullet));
}

void BulletManager::Update()
{
	//ƒtƒ‰ƒO‚ª‚½‚Á‚Ä‚È‚©‚Á‚½‚çÁ‚·
	bullets_.remove_if(
		[](auto& b) { return !b->GetIsAlive(); }
	);

	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}
}

void BulletManager::Draw()
{
	for (auto& bullet : bullets_)
	{
		bullet->Draw();
	}
}
