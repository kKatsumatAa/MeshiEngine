#include "PlayerAttackState.h"
#include "CollisionManager.h"
#include "PlayerHand.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

Vec3 PlayerAttackState::rotTmp_ = { 0,0,0 };


PlayerAttackState::~PlayerAttackState()
{
}

//何もしない---------------------------------------------------------------
void PlayerAttackStateNone::Initialize()
{
	//動いてない場合は判定とらない
	if (playerHand_->GetCollider())
	{
		playerHand_->GetCollider()->SetIsValid(false);
	}
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

	//判定有効に
	if (playerHand_->GetCollider())
	{
		playerHand_->GetCollider()->SetIsValid(true);
	}

	//手によって回転角度逆に
	if (playerHand_->GetIsRight())
	{
		rotTmp_.y = -PI;
	}
	else
	{
		rotTmp_.y = PI;
	}
	//ランダムに角度
	rotTmp_ = { GetRand(-PI / 8.0f,PI / 4.0f) ,rotTmp_.y / 2.0f,GetRand(-PI,PI) / 2.0f };
}

void PlayerAttackStateDoing::Update()
{
	float t = min((float)timer_ / (float)TIMER_MAX_, 1.0f);

	//経過時間で手を移動
	Vec3 addTrans = LerpVec3({ 0,0,0 }, playerHand_->GetFrontVecTmp() * (playerHand_->GetInterLength() - playerHand_->GetScale().z / 2.0f), EaseOut(t));

	//回転も
	Vec3 rot = LerpVec3({ 0,0,0 }, rotTmp_, EaseOut(t));
	playerHand_->SetRot(rot);


	//スピードをセット(ベクトルをプレイヤー本体角度で回転させて)
	Vec3 rotatedVec = addTrans;
	playerHand_->GetPlayer()->GetWorldMat()->CalcRotMat();
	playerHand_->SetVelocity(GetVec3xM4(addTrans, playerHand_->GetPlayer()->GetWorldMat()->GetRotMat(),0));

	//時間経過もゲームスピードをかける
	timer_ += 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//ステート変更
	if (t >= 1.0f)
	{
		//敵の被弾処理
		enemyDamageFunc_(playerHand_);

		//スピードをなくす
		playerHand_->SetVelocity({ 0,0,0 });

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
	Vec3 addTrans = LerpVec3(playerHand_->GetFrontVecTmp() * (playerHand_->GetInterLength() - playerHand_->GetScale().z / 2.0f), { 0,0,0 }, EaseOut(t));
	playerHand_->SetAddTrans(addTrans);

	//回転も
	Vec3 rot = LerpVec3(rotTmp_, { 0,0,0 }, EaseOut(t));
	playerHand_->SetRot(rot);

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