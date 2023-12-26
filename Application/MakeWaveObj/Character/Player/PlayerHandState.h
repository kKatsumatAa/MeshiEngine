/*
* @file PlayerHandState.h
* @brief プレイヤーの腕の状態
*/

#pragma once
#include"Util.h"
#include<functional>


class PlayerHand;

class PlayerHandState
{
public:
	const float ATTACK_HAND_ROT_MIN_ = -PI / 8.0f;
	const float ATTACK_HAND_ROT_MAX_ = PI / 4.0f;

protected:
	//状態を使うインスタンスポインタ
	PlayerHand* playerHand_ = nullptr;
	float timer_ = 0;

	//回転に使う基準値
	static Vec3 rotTmp_;

	//敵の被弾処理
	std::function<void(PlayerHand*)> enemyDamageFunc_ = nullptr;

public:
	//デストラクタ
	virtual ~PlayerHandState();

	//敵のダメージ処理関数をセット
	void SetEnemyDamageFunc(std::function<void(PlayerHand*)> f) { enemyDamageFunc_ = f; }
	//プレイヤーハンドのポインタセット
	void SetPlayerHand(PlayerHand* playerHand) { playerHand_ = playerHand; }

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
};

//攻撃していない
class PlayerHandStateNone : public PlayerHandState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//攻撃中
class PlayerHandStateDoing : public PlayerHandState
{
private:
	const int32_t TIMER_MAX_ = 5;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//攻撃中２
class PlayerHandStateDoing2 : public PlayerHandState
{
private:
	const int32_t TIMER_MAX_ = 11;

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};