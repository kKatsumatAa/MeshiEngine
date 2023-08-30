#include "PlayerHand.h"
#include "PlayerAttackState.h"
#include "Player.h"


std::unique_ptr<PlayerHand> PlayerHand::Create(Player* player, const Vec3& transTmp, bool isRight, const std::string& objName)
{
	std::unique_ptr<PlayerHand> instance = std::make_unique<PlayerHand>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(player, transTmp, isRight, objName))
	{
		assert(0);
	}

	return std::move(instance);
}

bool PlayerHand::Initialize(Player* player, const Vec3& transTmp, bool isRight, const std::string& objName)
{
	if (!Object::Initialize())
	{
		return false;
	}

	//当たり判定はないので属性もなし


	SetObjName(objName.c_str());

	//当たり判定に使わない
	SetColliderIsValid(false);

	player_ = player;
	isAttacking_ = false;
	//親子関係
	SetParent(player_);
	//仮でスケール
	SetScale({ 0.5f,0.5f,2.5f });
	//攻撃状態変更
	ChangeAttackState(std::make_unique<PlayerAttackStateNone>());

	//右手かどうか
	isRight_ = isRight;

	//元となる座標
	transTmp_ = transTmp;
	SetTrans(transTmp_);

	return true;
}


void PlayerHand::Update()
{
	attackState_->Update();

	//動かす
	SetTrans(transTmp_ + addTrans_);

	Object::Update();
}

void PlayerHand::Draw()
{
	attackState_->Draw();

	DrawCube3D(/*nullptr, { 0.2f,0.2f,0.2f,1.0f }*/);
}

void PlayerHand::ChangeAttackState(std::unique_ptr<PlayerAttackState> state)
{
	attackState_.reset();
	//所有権ごと渡す
	attackState_ = std::move(state);
	attackState_->SetPlayerHand(this);
	attackState_->Initialize();
}
