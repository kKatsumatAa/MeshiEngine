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
	uint16_t attribute = COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS;
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, attribute, &info_, player_->GetAttackLength());

	if (isRayHit)
	{
		info_.object->SetIsSilhouette(true);
		info_.object->Update();
	}
	return isRayHit;
}

void PlayerState::ThrowGun()
{
	player_->FallWeapon(player_->GetFrontVec() * 5.0f);

	//ゲームスピード加算
	GameVelocityManager::GetInstance().AddGameVelocity(0.9f);

	//ステート変更(素手)
	player_->ChangePlayerState(std::make_unique<PlayerStateBareHands>());
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
		if (info_.object->GetObjName() == "gun" && player_->GetWeapon() == nullptr && info_.object->GetParent() == nullptr
			&& MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			Gun* gun = dynamic_cast<Gun*>(info_.object);
			//銃拾う
			Vec3 localPos = { player_->GetScale().x_ ,-player_->GetScale().y_ / 2.0f ,player_->GetScale().z_ * 2.0f };
			player_->PickUpWeapon(gun, &localPos);

			//プレイヤーは逆向きなので仮に
			gun->SetRotY(PI);

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
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//クリックで撃つ
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			//弾がもうなければ投げる
			Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
			if (gun->GetBulletNum() <= 0)
			{
				ThrowGun();
				return;
			}

			player_->GetWeapon()->Attack(player_->GetFrontVec(), 1, player_);
			//ゲームスピード加算
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//右クリックで銃投げる
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			ThrowGun();
		}
	}
}


