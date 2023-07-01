#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Enemy :
	public Object
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 0.8f;
	bool isCantMove = false;

	//向きをプレイヤーに向けるためのクォータニオン
	Quaternion directionQua_;
	//初期向きのベクトル
	Vec3 directionVec = { 0,0,1.0f };
	//回転の時間
	float directionRotTime = 0;
	const float DIRCTION_ROT_TIME_ = 60;

	//hp
	int8_t hp = 3;
	float damageCoolTime = 0;
	float velocityLength = 0;

public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create(std::unique_ptr<WorldMat> worldMat);

private:
	//移動
	void Move();

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat) override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

