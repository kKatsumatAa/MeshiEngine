#include "PlayerAttackState.h"
#include "CollisionManager.h"
#include "PlayerHand.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"


//何もしない---------------------------------------------------------------
void PlayerAttackStateNone::Initialize()
{
}

void PlayerAttackStateNone::Update()
{

}

void PlayerAttackStateNone::Draw()
{
}


//攻撃1---------------------------------------------------------------
void PlayerAttackStateDoing::Initialize()
{
	playerHand_->SetIsAttacking(true);
}

void PlayerAttackStateDoing::Update()
{
	float t = (float)timer_ / (float)TIMER_MAX_;

	//経過時間で手を移動
	Vec3 addTrans = LerpVec3({ 0,0,0 }, Vec3(0, 0, 1.0f) * (playerHand_->GetInterLength() - playerHand_->GetScale().z_), EaseOut(t));

	playerHand_->SetAddTrans(addTrans);

	//時間経過もゲームスピードをかける
	timer_ += 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//ステート変更
	if (t >= 1.0f)
	{
		//敵の被弾処理
		enemyDamageFunc_();

		playerHand_->ChangeAttackState(std::make_unique<PlayerAttackStateDoing2>());
	}

}

void PlayerAttackStateDoing::Draw()
{
}


//攻撃2（戻す）---------------------------------------------------------------
void PlayerAttackStateDoing2::Initialize()
{
}

void PlayerAttackStateDoing2::Update()
{
	float t = (float)timer_ / (float)TIMER_MAX_;

	//経過時間で手を戻す
	Vec3 addTrans = LerpVec3(Vec3(0, 0, 1.0f) * (playerHand_->GetInterLength() - playerHand_->GetScale().z_), { 0,0,0 }, EaseOut(t));

	playerHand_->SetAddTrans(addTrans);

	//時間経過もゲームスピードをかける
	timer_ += 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//ステート変更
	if (t >= 1.0f)
	{
		playerHand_->SetIsAttacking(false);

		playerHand_->ChangeAttackState(std::make_unique<PlayerAttackStateNone>());
	}
}

void PlayerAttackStateDoing2::Draw()
{
}
