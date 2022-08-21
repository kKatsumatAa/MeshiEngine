#pragma once
#include"Vec3.h"
#include"Vec2.h"
#include"CollisionConfig.h"
#include <cstdint>

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


public:
	bool isBoss = false;
	int coolTime = 0;

	int HP=0;

	float radius_ = 5.0f;
	//ロックオンモードかどうか(player用→当たり判定の時に使用)
	bool isLockOn = false;
	//自分がロックオンしている敵の数（player用）
	int isLockNum = 0;
	//死亡フラグ
	bool isDead = false;
	//ロックオンされているかどうか（敵用）
	int isLockOned = 0;

	float GetRadius();
	void SetRadius(const float& radius);

	//衝突時に呼ばれる
	virtual void OnCollision() = 0;
	virtual void OnCollision2() = 0;
	//world座標をゲット
	virtual Vec3 GetWorldPos() = 0;

	//衝突用ビット系
	//自分
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(const uint32_t& bit);
	//敵
	uint32_t GetCollisionMask();
	void SetCollisionMask(const uint32_t& bit);
};

