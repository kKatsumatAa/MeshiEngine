#pragma once
#include"Collider.h"

class ReloadEffect
{
private:
	Object object;
	WorldMat worldTransform;
	//õ–½
	int32_t lifeTime = 0;
	int32_t deathTimer_ = 0;
	Vec3 endPos;
	Vec3 startPos;

	bool isDead = false;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	void Initialize(Vec3 startPos, Vec3 endPos, int time, float width);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
};


class ReloadEffectManager
{
private:
	ReloadEffectManager();
	~ReloadEffectManager();

public:
	//
	std::list< std::unique_ptr<ReloadEffect>> reloadEffects_;


	void Initialize();

	void GenerateReloadEffect(Vec3 startPos, Vec3 endPos, int time, float width);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//ƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<ReloadEffect>>& GetReloadEffects()
	{
		return reloadEffects_;
	}

	static ReloadEffectManager& GetInstance();
};




