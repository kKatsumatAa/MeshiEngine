#pragma once
#include"Object.h"

class Player;
class PlayerAttackState;


class PlayerHand : public Object
{
private:
	bool isAttacking_ = false;
	std::unique_ptr<PlayerAttackState> attackState_ = nullptr;
	Player* player_ = nullptr;

	//敵の衝突点までの距離
	float interLength_ = 0;

	//右手か
	bool isRight_ = false;

	//攻撃時に足す用のポジション
	Vec3 addTrans_ = { 0,0,0 };
	//元となる座標
	Vec3 transTmp_ = { 0,0,0 };

public:
	void SetPlayer(Player* player) { player_ = player; }
	Player* GetPlayer() { return player_; }

	void SetIsAttacking(bool is) { isAttacking_ = is; }
	bool GetIsAttacking() { return isAttacking_; }

	//敵の衝突点までの距離
	void SetInterLength(float length) { interLength_ = length; }
	float GetInterLength() { return interLength_; }

	void SetAddTrans(const Vec3& addTrans) { addTrans_ = addTrans; }
	const Vec3& GetAddTrans() { return addTrans_; }

	//ステート
	PlayerAttackState* GetAttackState() { return attackState_.get(); }


public:
	static std::unique_ptr<PlayerHand> Create(Player* player, const Vec3& transTmp,bool isRight, const std::string& objName);
	bool Initialize(Player* player, const Vec3& transTmp,bool isRight, const std::string& objName);

	void Update()override;

	void Draw()override;


	//攻撃のステート変更
	void ChangeAttackState(std::unique_ptr<PlayerAttackState> state);
};
