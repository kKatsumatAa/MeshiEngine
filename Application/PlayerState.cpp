#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"



bool PlayerState::CheckEyeRayHit()
{
	//攻撃できるかどうか
//レイにプレイヤーの正面ベクトル入れる
	Ray ray;
	ray.dir = { player_->GetFrontVec().x_,player_->GetFrontVec().y_,player_->GetFrontVec().z_ };
	ray.start = { player_->GetTrans().x_,player_->GetTrans().y_,player_->GetTrans().z_ };

	//正面ベクトルに何かあるか
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, "player", &info_, player_->GetAttackLength());

	return isRayHit;
}


//素手状態-----------------------------------------------------------------------
void PlayerStateBareHands::Initialize()
{
}

void PlayerStateBareHands::Update()
{
	bool isRayHit = CheckEyeRayHit();

	//何かしら照準にあったら
	if (isRayHit)
	{
		player_->SetIsTarget(true);

		//銃を持ってなくて持ち主がいない銃に照準があってたら
		if (info_.object->GetObjName() == "gun" && player_->GetGun() == nullptr && info_.object->GetParent() == nullptr
			&& MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			Gun* gun = dynamic_cast<Gun*>(info_.object);
			player_->SetGun(gun);
			Vec3 localP = { player_->GetScale().x_ ,-player_->GetScale().y_ * 2.0f ,player_->GetScale().z_ * 2.0f };
			gun->SetLocalPos(localP);
			gun->ChangeOwner(player_->GetWorldMat());

			//ステート変更(銃)
			player_->ChangePlayerState(std::make_unique<PlayerStateHaveGun>());
		}
		//敵が照準にあったら殴る
		else if (info_.object->GetObjName() == "enemy")
		{
			player_->GetHandManager()->Attack(info_);
		}
	}
	//なければ
	else
	{
		player_->SetIsTarget(false);
	}
}


//銃持ってる状態-----------------------------------------------------------------------
void PlayerStateHaveGun::Initialize()
{
}

void PlayerStateHaveGun::Update()
{
	//プレイヤーが銃を持っていたら
	if (player_->GetGun() != nullptr)
	{
		player_->SetIsTarget(false);

		//クリックで撃つ
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			player_->GetGun()->Shot(player_->GetFrontVec());
			//ゲームスピード加算
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//右クリックで銃投げる
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			player_->GetGun()->ChangeOwner(nullptr);
			player_->GetGun()->SetFallVec(player_->GetFrontVec() * 10.0f);
			player_->SetGun(nullptr);

			//ステート変更(素手)
			player_->ChangePlayerState(std::make_unique<PlayerStateBareHands>());
		}
	}
}


