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
		info_.object->EffectUpdate();
	}
	return isRayHit;
}

void PlayerState::ThrowWeapon()
{
	player_->FallWeapon(player_->GetFrontVec() * 8.0f);

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

		//武器を持ってなくて持ち主がいない武器に照準があってたら
		if ((info_.object->GetObjName() == "gun" || info_.object->GetObjName() == "sword")
			&& player_->GetWeapon() == nullptr && info_.object->GetParent() == nullptr
			&& MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			Weapon* weapon = dynamic_cast<Weapon*>(info_.object);
			//武器拾う
			Vec3 localPos = { player_->GetScale().x_ ,-player_->GetScale().y_ / 2.0f ,player_->GetScale().z_ * 2.0f };
			player_->PickUpWeapon(weapon, &localPos);

			//プレイヤーは逆向きなので仮に
			weapon->SetRotY(PI);

			//ステート変更
			player_->ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
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


//武器持ってる状態-----------------------------------------------------------------------
void PlayerStateHaveWeapon::Initialize()
{
}

void PlayerStateHaveWeapon::Update()
{
	//プレイヤーが銃を持っていたら
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//クリックで攻撃
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			//弾がもうなければ投げる
			if (player_->GetWeapon()->GetObjName() == "gun")
			{
				Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
				if (gun->GetBulletNum() <= 0)
				{
					ThrowWeapon();
					return;
				}
			}

			//攻撃
			player_->GetWeapon()->Attack(player_->GetFrontVec(), 1, player_);
			//ゲームスピード加算
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//右クリックで武器投げる
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			ThrowWeapon();
		}
	}
}


