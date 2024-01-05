/*
* @file EnemyStanceState.h
* @brief 敵の構えのステート（銃を構えてるか、殴りの構えか）
*/

#pragma once
#include"EnemyState.h"

//-------------------------
//攻撃構えの親ステート
class EnemyStateAttackStance :
	public EnemyState
{
public:
	const float TIME_RATIO_MAX_ = 1.0f;
	//最大時間
	const float TIMER_MAX_ = 40.0f;
	const std::string MOVE_NODE_NAME_ = "Arm";

protected:
	//imguiの角度のドラッグスピード
	static const float ANGLE_IMGUI_DRAG_SPEED_;

	float timeRatio_ = 0;
	//足す角度
	static Vec3 ANGLE_MAX_;
	//線形補間用の角度
	Vec3 stanceEndRot_ = { 0,0,0 };
	Vec3 stanceBeginRot_ = { 0,0,0 };

public:
	//初期化
	virtual void Initialize() override = 0;
	//更新
	virtual void Update() override;
	//ImGui描画
	static void DrawImgui();
protected:
	//武器を持ってるのが右か左か
	std::string GetHaveWeaponLeftRight();
};

//銃の攻撃構え始め
class EnemyStateAttackStanceBegin :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceBegin
{
public:
	const float TIME_RATIO_MAX_ = 1.0f;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//銃の攻撃構え終わり
class EnemyStateAttackStanceEnd :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//----------------------
//素手攻撃何もなし
class EnemyStateBareHandsAttackNone :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
private:
	const float TIME_ = 20.0f;
	float t_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//素手攻撃状態始め
class EnemyStateBareHandsAttackBegin :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
private:
	const float TIME_ = 20.0f;
	float t_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};


//素手攻撃状態
class EnemyStateBareHandsAttack :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
private:
	const float ATTACK_TIME_ = 45.0f;
	float t_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};

//素手攻撃おわり
class EnemyStateBareHandsAttackEnd :
	public EnemyStateAttackStance,
	public CharacterStateAttackStanceEnd
{
private:
	const float TIME_MAX_ = 20.0f;
	float timeRatio_ = 0;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
};