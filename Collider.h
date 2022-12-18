#pragma once
#include"Vec3.h"
#include"Vec2.h"
#include"CollisionConfig.h"
#include"Model.h"
#include"KeyboardInput.h"
#include"DebugText.h"
#include"Util.h"
#include "Assert.h"
#include "WindowsApp.h"
#include <random>
#include "Sound.h"

//仮
	//乱数シード生成器
static std::random_device seed_gen;
//メルセンヌツイスター
static std::mt19937_64 engine(seed_gen());
//乱数範囲
static std::uniform_real_distribution<float> posDistX(-5.0, 5.0f);
static std::uniform_real_distribution<float> posDistY(-10.0f, 10.0f);

/// <summary>
/// 衝突判定オブジェクト
/// </summary>
class Collider
{
private:
	//衝突属性(自分)
	uint32_t collisionAttribute_ = 0xffffffff;
	//衝突属性(相手)
	uint32_t collisionMask_ = 0xffffffff;



protected:
	float radius_ = 1.0f;
	//死亡フラグ
	bool isDead = false;

	WorldMat worldTransform_;

	//プレーヤーかどうか（判定時に使う）
	bool isPlayer = false;

	int HP = 0;

	//敵の赤い部分（ダメージ受ける部分）
	float enemyRedRate = 0;

public:
	bool GetIsDead() { return isDead; }
	void SetIsDead(bool isDead) { this->isDead = isDead; }

	int GetHP() { return HP; }
	void SetHP(int hp) { HP = hp; }
	float GetRadius() { return radius_; };
	bool GetIsPlayer() { return isPlayer; }
	void SetRadius(const float& radius) { this->radius_ = radius; };
	float GetEnemyRedRate() { return enemyRedRate; }
	void SetEnemyRedRate(float enemyRedRate) { this->enemyRedRate = enemyRedRate; }

	//衝突時に呼ばれる
	virtual void OnCollision(Collider& collider) = 0;
	//手と敵の判定用
	virtual void OnCollision2(Collider& collider) = 0;

	void SetWorldPos(Vec3 pos) { worldTransform_.trans = pos; }//
	Vec3 GetWorldPos() { return worldTransform_.trans; }
	WorldMat* GetWorldTransForm() { return &worldTransform_; }

	//衝突用ビット系
	//自分
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(const uint32_t& bit);
	//敵
	uint32_t GetCollisionMask();
	void SetCollisionMask(const uint32_t& bit);
};

