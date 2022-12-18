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

public:
	bool GetIsDead() { return isDead; }

	int GetHP() { return HP; }
	float GetRadius() { return radius_; };
	bool GetIsPlayer() { return isPlayer; }
	void SetRadius(const float& radius) { this->radius_ = radius; };

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

