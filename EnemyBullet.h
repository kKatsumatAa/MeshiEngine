#pragma once
#include <cassert>
#include "Util.h"
#include "Player.h"

class EnemyBullet : public Collider
{
private:
	WorldMat worldMat;
	Draw draw;
	//����
	static const int32_t lifeTime = 60 * 5;
	int32_t deathTimer_ = lifeTime;
	bool isDead_ = false;
	Player* player_;
public:
	Vec3 velocity_;

public:
	void Initialize(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection,const UINT64& texHundle);
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }

	Vec3 GetWorldPos() override;

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision() override;
};

