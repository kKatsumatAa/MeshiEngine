#pragma once
#include "Collider.h"

class PlayerBullet : public Collider
{
private:

	Model* model = nullptr;
	Object draw;
	//寿命
	static const int32_t lifeTime = 60;
	int32_t deathTimer_ = lifeTime;

	const Vec3 scaleTmp = { 1.0f,1.0f,1.0f };

	int count = 0;

	Vec3 velocityTmp;

public:


public:
	void Initialize(const Vec3& position, const Vec3& velocity, Model* model);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision(Collider& collider) override;
	void OnCollision2(Collider& collider)override;
};

