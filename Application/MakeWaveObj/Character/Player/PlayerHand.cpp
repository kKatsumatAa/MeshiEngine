#include "PlayerHand.h"
#include "PlayerHandState.h"
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
		assert(false);
	}

	return std::move(instance);
}

bool PlayerHand::Initialize(Player* player, const Vec3& transTmp, bool isRight, const std::string& objName)
{
	if (!Object::Initialize())
	{
		return false;
	}

	SetObjName(objName.c_str());

	//コライダー
	SetCollider(std::make_unique<SphereCollider>());
	//当たり判定はいったん切る
	GetCollider()->SetIsValid(false);
	GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	player_ = player;
	isAttacking_ = false;
	//親子関係
	SetParent(player_);
	//仮でスケール
	SetScale(SCALE_);
	//攻撃状態変更
	ChangeAttackState(std::make_unique<PlayerHandStateNone>());

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

	DrawCube3D();
}

void PlayerHand::ChangeAttackState(std::unique_ptr<PlayerHandState> state)
{
	attackState_.reset();
	//所有権ごと渡す
	attackState_ = std::move(state);
	attackState_->SetPlayerHand(this);
	attackState_->Initialize();
}
