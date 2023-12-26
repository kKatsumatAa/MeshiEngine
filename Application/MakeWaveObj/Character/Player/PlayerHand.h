/*
* @file PlayerHand.h
* @brief プレイヤーの腕
*/

#pragma once
#include"Object.h"

class Player;
class PlayerHandState;


class PlayerHand : public Object
{
public:
	const Vec3 SCALE_ = { 0.5f,0.5f,2.5f };

private:
	bool isAttacking_ = false;
	std::unique_ptr<PlayerHandState> attackState_ = nullptr;
	Player* player_ = nullptr;

	//敵の衝突点までの距離
	float interLength_ = 0;

	//右手か
	bool isRight_ = false;

	//攻撃時に足す用のポジション
	Vec3 addTrans_ = { 0,0,0 };
	//元となる座標
	Vec3 transTmp_ = { 0,0,0 };

	//攻撃できる状態か
	bool isCanAttack_ = false;

public:
	//プレイヤーポインタセット
	void SetPlayer(Player* player) { player_ = player; }
	//プレイヤーポインタ取得
	Player* GetPlayer() { return player_; }

	//攻撃中かセット
	void SetIsAttacking(bool is) { isAttacking_ = is; }
	//攻撃中か取得
	bool GetIsAttacking() { return isAttacking_; }

	//攻撃できるかセット
	void SetIsCanAttack(bool is) { isCanAttack_ = is; }
	//攻撃できるか取得
	bool GetIsCanAttack() { return isCanAttack_; }

	//敵の衝突点までの距離セット
	void SetInterLength(float length) { interLength_ = length; }
	//敵の衝突点までの距離取得
	float GetInterLength() { return interLength_; }

	//攻撃時の加算する位置セット
	void SetAddTrans(const Vec3& addTrans) { addTrans_ = addTrans; }
	//攻撃時の加算する位置取得
	const Vec3& GetAddTrans() { return addTrans_; }

	//右腕かどうか
	bool GetIsRight() { return isRight_; }

	//ステート
	PlayerHandState* GetAttackState() { return attackState_.get(); }


public:
	//インスタンス生成処理
	static std::unique_ptr<PlayerHand> Create(Player* player, const Vec3& transTmp, bool isRight, const std::string& objName);
	//初期化
	bool Initialize(Player* player, const Vec3& transTmp, bool isRight, const std::string& objName);
	//更新
	void Update()override;
	//描画
	void Draw()override;

public:
	//攻撃のステート変更
	void ChangeAttackState(std::unique_ptr<PlayerHandState> state);
};
