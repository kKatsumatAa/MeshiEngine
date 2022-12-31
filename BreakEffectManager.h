#pragma once

#include"Collider.h"

class BreakEffect
{
private:
	Object object;
	WorldMat worldTransform;
	Vec3 velocity;
	float gravity;
	//õ–½
	static const int32_t lifeTime = 180;
	int32_t deathTimer_ = lifeTime;

	bool isDead = false;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	void Initialize(Vec3 pos, Vec3 velocity, float gravity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
};


class BreakEffectManager
{
private:

public:
	//
	std::list< std::unique_ptr<BreakEffect>> breakEffects_;


	void Initialize();

	void GenerateBreakEffect(Vec3 pos);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//ƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<BreakEffect>>& GetBreakEffects()
	{
		return breakEffects_;
	}

	void GenerateRandomEffect(Vec3 pos, int num);
};



