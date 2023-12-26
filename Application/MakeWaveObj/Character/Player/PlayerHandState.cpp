#include "PlayerHandState.h"
#include "CollisionManager.h"
#include "PlayerHand.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

Vec3 PlayerHandState::rotTmp_ = { 0,0,0 };


PlayerHandState::~PlayerHandState()
{
}

//何もしない---------------------------------------------------------------
void PlayerHandStateNone::Initialize()
{
	//動いてない場合は判定とらない
	if (playerHand_->GetCollider())
	{
		playerHand_->GetCollider()->SetIsValid(false);
	}
}

void PlayerHandStateNone::Update()
{

}

void PlayerHandStateNone::Draw()
{
}


//攻撃1---------------------------------------------------------------
void PlayerHandStateDoing::Initialize()
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
	rotTmp_ = { GetRand(ATTACK_HAND_ROT_MIN_,ATTACK_HAND_ROT_MAX_) ,rotTmp_.y / 2.0f,GetRand(-PI,PI) / 2.0f };
}

void PlayerHandStateDoing::Update()
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
	playerHand_->SetVelocity(GetVec3xM4(addTrans, playerHand_->GetPlayer()->GetWorldMat()->GetRotMat(), 0));

	//時間経過もゲームスピードをかける
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	//ステート変更
	if (t >= 1.0f)
	{
		//敵の被弾処理
		enemyDamageFunc_(playerHand_);

		//スピードをなくす
		playerHand_->SetVelocity({ 0,0,0 });

		playerHand_->ChangeAttackState(std::make_unique<PlayerHandStateDoing2>());
	}

}

void PlayerHandStateDoing::Draw()
{
}


//攻撃2（戻す）---------------------------------------------------------------
void PlayerHandStateDoing2::Initialize()
{
}

void PlayerHandStateDoing2::Update()
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

		playerHand_->ChangeAttackState(std::make_unique<PlayerHandStateNone>());
	}
}

void PlayerHandStateDoing2::Draw()
{
}