#pragma once

#include"Collider.h"

class CartridgeEffect
{
private:
	Object object;
	WorldMat worldTransform;
	Vec3 velocity;
	float gravity;
	bool isLeft;
	//õ–½
	static const int32_t lifeTime = 60;
	int32_t deathTimer_ = lifeTime;

	bool isDead = false;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	void Initialize(Vec3 pos, float power, float gravity, bool isLeft);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
};


class CartridgeEffectManager
{
private:

public:
	//
	std::list< std::unique_ptr<CartridgeEffect>> cartridgeEffects_;


	void Initialize();

	void GenerateCartridgeEffect(Vec3 pos);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//ƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<CartridgeEffect>>& GetCartridgeEffects()
	{
		return cartridgeEffects_;
	}
};



