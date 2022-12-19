#include "PlayerBulletManager.h"

void PlayerBulletManager::Initialize(Model* model)
{
	assert(model);

	if (texhandle == NULL)
	{
		TextureManager::LoadGraph(L"Resources/image/white.png", texhandle);
	}

	this->model = model;
	playerBullets_.clear();
}

void PlayerBulletManager::GeneratePlayerBullet(const Vec3& position, const Vec3& velocity)
{
	//ãÖÇê∂ê¨ÅAèâä˙âª
	std::unique_ptr<PlayerBullet> playerBullet = std::make_unique<PlayerBullet>();
	playerBullet->Initialize(position, velocity, model);
	//ãÖÇìoò^
	playerBullets_.push_back(std::move(playerBullet));
}

void PlayerBulletManager::Update()
{
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_)
	{
		bullet->Update();
	}

	playerBullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet)
		{
			return bullet->GetIsDead();
		}
	);
}

void PlayerBulletManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_)
	{
		bullet->Draw(view, projection);
	}
}

void PlayerBulletManager::DrawSprite()
{
	gauge[1].DrawClippingBoxSprite({ 1100,100,0 }, 1.0f, { 0,0 }, { 0.5f,2.0f }, { 0.3f,0.3f,0.3f,1.0f },
		texhandle);
	gauge[0].DrawClippingBoxSprite({1100,100,0}, 1.0f, {0,0}, {0.5f,(float)bulletNum / (float)bulletNumMax * 2.0f}, {0.7f,0.7f,0.7f,0.9f},
		texhandle);
}
