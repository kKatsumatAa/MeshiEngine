#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "ObjectManager.h"
#include "CameraManager.h"



bool PlayerState::CheckEyeRayHit()
{
	bool isRayHit = player_->CheckRayOfEyeHit(player_->GetFrontVec(),
		player_->GetAttackLength(), COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS, &info_);

	//シルエットオフ
	auto objs = ObjectManager::GetInstance().GetObjs();
	for (auto obj : objs)
	{
		obj->SetIsSilhouette(false);
	}

	//シルエット
	if (isRayHit)
	{
		info_.object->SetIsSilhouette(true);
		info_.object->EffectUpdate();
	}

	return isRayHit;
}

void PlayerState::Update()
{
	//カメラの向き変更
	player_->DirectionUpdate();

	//移動
	player_->Move();
}


//素手状態-----------------------------------------------------------------------
void PlayerStateBareHands::Initialize()
{
}

void PlayerStateBareHands::Update()
{
	PlayerState::Update();

	bool isRayHit = PlayerState::CheckEyeRayHit();

	//何かしら照準にあったら
	if (isRayHit)
	{
		player_->SetIsTarget(true);

		//武器を持ってなくて持ち主がいない武器に照準があってたら
		if ((info_.object->GetObjName() == "gun" || info_.object->GetObjName() == "sword")
			&& player_->GetWeapon() == nullptr && info_.object->GetParent() == nullptr)
		{
			//ui変更
			PlayerUI::GetInstance().ChangeState("PICKUP");

			if (player_->GetIsClickLeft())
			{
				Weapon* weapon = dynamic_cast<Weapon*>(info_.object);
				//武器拾う
				Vec3 localPos = player_->GetWeaponPosTmp();
				weapon->SetRot({ 0,0,0 });
				player_->PickUpWeapon(weapon, &localPos);

				//ui変更
				PlayerUI::GetInstance().ChangeState("GUN");

				//ステート変更
				player_->ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
			}
		}
		//敵が照準にあったら殴る
		else if (info_.collider->GetAttribute() & COLLISION_ATTR_ENEMYS)
		{
			//ui変更
			PlayerUI::GetInstance().ChangeState("PUNCH");

			//攻撃する
			if (player_->GetIsClickLeft())
			{
				player_->GetHandManager()->Attack(info_);
			}
		}
	}
	//なければ
	else
	{
		//ui変更
		PlayerUI::GetInstance().ChangeState("NORMAL");

		player_->SetIsTarget(false);
	}
}


//武器持ってる状態-----------------------------------------------------------------------
void PlayerStateHaveWeapon::Initialize()
{
	//仮でui変更
	PlayerUI::GetInstance().ChangeState("GUN");
}

void PlayerStateHaveWeapon::Update()
{
	PlayerState::Update();

	//プレイヤーが武器を持っていたら
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//銃のクールタイム
		if (player_->GetWeapon()->GetObjName() == "gun")
		{
			//クールタイムでUI回転
			PlayerUI::GetInstance().SetAngle(-PI * 2.0f * player_->GetWeapon()->GetAttackCoolTimeRatio());
			//銃角度
			player_->GetWeapon()->SetRotX(LerpVec3({ PI / 4.0f,0,0 }, { 0,0,0 },
				EaseInOutBack(player_->GetWeapon()->GetAttackCoolTimeRatio())).x);
		}

		//クリックで攻撃
		if (player_->GetIsClickLeft())
		{
			//弾がもうなければ投げる
			if (player_->GetWeapon()->GetObjName() == "gun")
			{
				Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
				if (gun->GetBulletNum() <= 0)
				{
					player_->ThrowWeapon();
					return;
				}
			}

			Vec3 targetDir = player_->GetFrontVec();
			//レイを飛ばしてターゲットまでのベクトル取得
			RaycastHit info;
			if (player_->CheckRayOfEyeHit(player_->GetFrontVec(), 10000, COLLISION_ATTR_ALL ^ COLLISION_ATTR_ALLIES, &info))
			{
				targetDir = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - (
					player_->GetWeapon()->GetWorldTrans());
				targetDir.Normalized();
			}

			//攻撃
			player_->GetWeapon()->Attack(targetDir, 1, player_, PARTICLE_SIZE_EXTEND_);
			//ゲームスピード加算
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//右クリックで武器投げる
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			player_->ThrowWeapon();
		}
	}
	//壊されたりしてなくなったら
	else
	{
		//ステート変更(素手)
		player_->ChangePlayerState(std::make_unique<PlayerStateBareHands>());
	}
}


//-----------------------------------------------------------------------------------------
//死亡演出
void PlayerStateDeadEffect::Initialize()
{
	targetPos_ = player_->GetBulletOwnerEnemyPos();
	dir_ = (targetPos_ - player_->GetWorldTrans()).GetNormalized();

	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");
	camera->SetTarget(targetPos_);
}

void PlayerStateDeadEffect::Update()
{
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	t_ = (float)timer_ / player_->GetDeadTimerMax();

	camera->SetEye(LerpVec3(targetPos_ - (dir_ * LENGTH_MIN_), player_->GetWorldTrans(), EaseInOut(t_)));

	//演出終わったら生存フラグオフ
	if (timer_ >= player_->GetDeadTimerMax())
	{
		player_->SetIsAlive(false);
	}

	timer_++;
}
