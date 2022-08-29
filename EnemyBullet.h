#pragma once
#include <cassert>
#include "Util.h"
#include "Player.h"

enum
{
	NORM,
	HOMING
};

class EnemyBullet : public Collider
{
private:
	
	Draw draw;
	//����
	static const int32_t lifeTime = 60 * 6;
	int32_t deathTimer_ = lifeTime;
	Player* player_;
public:
	Vec3 velocity_;
	int BulletVariation = NORM;
	WorldMat worldMat;

public:
	void Initialize(const Vec3& position, const Vec3& velocity, int BulletVariation = NORM);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection,const UINT64& texHundle);
	bool IsDead() const { return isDead; }
	void SetPlayer(Player* player) { player_ = player; }

	Vec3 GetWorldPos() override;

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision() override;
	void OnCollision2()override;
};

