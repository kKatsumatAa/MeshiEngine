#pragma once
#include "Collider.h"

class PlayerBullet : public Collider
{
private:

	Model* model = nullptr;
	Draw draw;
	//����
	static const int32_t lifeTime = 60;
	int32_t deathTimer_ = lifeTime;

	const Vec3 scaleTmp = { 1.0f,1.0f,1.0f };

	Vec3 velocity = { 0,0,0 };

	int count = 0;

public:


public:
	void Initialize(const Vec3& position, const Vec3& velocity, Model* model);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision(Collider& collider) override;
	void OnCollision2(Collider& collider)override;
};

