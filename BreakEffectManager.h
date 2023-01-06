#pragma once

#include"Collider.h"

class BreakEffect
{
private:
	Object object;
	WorldMat worldTransform;
	Vec3 velocity;
	float gravity;
	//寿命
	static const int32_t lifeTime = 180;
	int32_t deathTimer_ = lifeTime;

	bool isDead = false;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	void Initialize(Vec3 pos, Vec3 velocity, float gravity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, UINT64 texHandle);
};


class BreakEffectManager
{
private:
	UINT64 texHandle[10];

public:
	//
	std::list< std::unique_ptr<BreakEffect>> breakEffects_;


	void Initialize();

	void GenerateBreakEffect(Vec3 pos);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//リストを取得(const参照)
	const std::list<std::unique_ptr<BreakEffect>>& GetBreakEffects()
	{
		return breakEffects_;
	}

	void GenerateRandomEffect(Vec3 pos, int num);
};



