#pragma once
#include"Draw.h"
#include"worldMat.h"
#include"KeyboardInput.h"
#include"ViewMat.h"
#include"ProjectionMat.h"
#include"Collider.h"
#include <iostream>
#include <list>

class PlayerBullet : public Collider
{
private:
	WorldMat worldMat;
	Draw draw;
	//寿命
	static const int32_t lifeTime = 60 * 5;
	int32_t deathTimer_ = lifeTime;
	float scale = 0.5f;
public:
	Vec3 velocity_;

public:
	void Initialize(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle);
	bool IsDead() const { return isDead; }

	Vec3 GetWorldPos() override;

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision() override;
	void OnCollision2()override;
};

