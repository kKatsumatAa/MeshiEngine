#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"

class PlayerAttackState;

class Player :
	public Object
{
private:
	const float ANGLE_VEL_EXTEND_ = 0.003f;
	Vec3 cameraRot_ = { 0,0,0 };

	Vec3 frontVec_ = { 0,0,1.0f };
	Vec3 upVec_ = { 0,1.0f,0 };
	Vec3 rightVec_ = { 0,0,0 };

	//攻撃中か
	bool isAttacking_ = false;
	//攻撃可能か
	bool isCanAttack_ = false;

	//攻撃が届く距離
	const float attackLength_ = 20.0f;

	//攻撃のステート
	std::unique_ptr<PlayerAttackState> attackState_ = nullptr;

public:


public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Player> Create();

private:
	//移動
	void Move();
	//向きを変える
	void DirectionUpdate();

public:
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	void SetIsCanAttack(bool is) { isCanAttack_ = is; }

	bool GetIsAttacking() { return isAttacking_; }
	bool GetIsCanAttack() { return isCanAttack_; }

	float GetAttackLength() { return attackLength_; }

public:

	bool Initialize() override;

	void Update() override;

	void Draw() override;

	//攻撃のステート変更
	void ChangeAttackState(std::unique_ptr<PlayerAttackState> state);

	void OnCollision(const CollisionInfo& info) override;
};

