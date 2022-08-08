#pragma once
#include"Draw.h"
#include"worldMat.h"
#include"KeyboardInput.h"
#include"ViewMat.h"
#include"ProjectionMat.h"
#include <iostream>
#include <list>

class PlayerBullet
{
private:
	WorldMat worldMat;
	Draw draw;
	//����
	static const int32_t lifeTime = 60 * 5;
	int32_t deathTimer_ = lifeTime;
	bool isDead_ = false;
	float scale = 0.5f;
public:
	Vec3 velocity_;

public:
	void Initialize(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle);
	bool IsDead() const { return isDead_; }
};

