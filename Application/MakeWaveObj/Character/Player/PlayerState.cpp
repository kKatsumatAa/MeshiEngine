#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PostEffectManager.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LevelManager.h"
#include "EnemyPart.h"



bool PlayerState::CheckEyeRayHit()
{
	//正面ベクトルに何かあるか
	bool isRayHit = player_->CheckRayOfEyeHit(player_->GetFrontVec(),
		player_->GetAttackLength(), COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS, &info_);

	//シルエットオフ
	auto objs = ObjectManager::GetInstance().GetObjs();
	for (auto obj : objs)
	{
		obj->SetIsSilhouette(false);
	}
	//敵メッシュの色の倍率オフ
	auto enemies = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "enemy2.fbx");
	for (auto enemy : enemies)
	{
		enemy->GetModel()->SetMeshColorRate({ 1.0f,1.0f,1.0f,1.0f });
	}

	if (isRayHit)
	{
		//シルエット
		info_.object->SetIsSilhouette(true);
		info_.object->EffectUpdate();
		
		//相手の当たり判定処理呼ぶ
		CollisionInfo infoL;
		std::string colInfoObjName = "PlayerRay";
		infoL.objName_ = &colInfoObjName;
		info_.object->OnCollision(infoL);
	}

	return isRayHit;
}

void PlayerState::Update()
{
	if (commonUpdateF_)
	{
		commonUpdateF_();
	}
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
		if ((info_.object->GetObjName() == "gun" || info_.object->GetObjName() == EnemyPart::S_OBJ_NAME_)
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
			player_->GetWeapon()->SetRotX(Lerp(SHOOT_ROT_MAX_, 0,
				EaseInOutBack(player_->GetWeapon()->GetAttackCoolTimeRatio())));
		}

		//クリックで攻撃
		if (player_->GetIsClickLeft())
		{
			//銃で弾がもうないなら投げる
			if (player_->GetWeapon()->GetObjName() == "gun")
			{
				Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
				if (gun->GetBulletNum() <= 0)
				{
					player_->ThrowWeapon();
					return;
				}
			}
			//敵の部位なら投げる
			else if (player_->GetWeapon()->GetObjName() == EnemyPart::S_OBJ_NAME_)
			{
				player_->ThrowWeapon();
				return;
			}

			Vec3 targetDir = player_->GetFrontVec();
			//レイを飛ばしてターゲットまでのベクトル取得
			targetDir = player_->GetFrontTargetVec(COLLISION_ATTR_ALL ^ COLLISION_ATTR_ALLIES);

			//攻撃
			player_->GetWeapon()->Attack(targetDir, player_->DECREMENT_BULLET_NUM_, player_, PARTICLE_SIZE_EXTEND_);
			//ゲームスピード加算
			GameVelocityManager::GetInstance().AddGameVelocity(GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_);
		}
		//右クリックで武器投げる
		else if (player_->GetIsClickRight())
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
	targetPos_ = player_->GetPosOfEnemyAttack();
	dir_ = (targetPos_ - player_->GetWorldTrans()).GetNormalized();

	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");
	camera->SetTarget(targetPos_);
}

void PlayerStateDeadEffect::Update()
{
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	t_ = (float)timer_ / player_->GetDeadEffectTimerMax();

	camera->SetEye(LerpVec3(targetPos_ - (dir_ * LENGTH_MIN_), player_->GetWorldTrans(), EaseInOut(t_)));

	//演出終わったら生存フラグオフ
	if (timer_ >= player_->GetDeadEffectTimerMax())
	{
		player_->SetIsAlive(false);
	}

	timer_++;
}


//-----------------------------------------------------------------------------------------
//死亡演出2(マグマで)
void PlayerStateDeadEffect2::Initialize()
{
	beginRot_ = player_->GetCameraRot();
}

void PlayerStateDeadEffect2::Update()
{
	t_ = (float)timer_ / player_->GetDeadEffectTimerMax();

	//ゆっくりにする
	GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(true);

	Vec3 rot = LerpVec3(beginRot_, { endRotX_,beginRot_.y,beginRot_.z }, EaseInOutBack(t_));
	//正面ベクトルを回転、そのベクトルをカメラのターゲットに使う
	player_->RotateFrontVec(rot);
	player_->UpdateUseCameraTarget();

	//演出終わったら生存フラグオフ
	if (timer_ >= player_->GetDeadEffectTimerMax())
	{
		player_->SetIsAlive(false);
	}

	timer_++;
}


//--------------------------------------------------------------------------------------
//殴られて死んだら
void PlayerStateDeadEffectPunched::Initialize()
{
	beginRot_ = player_->GetCameraRot();
	beginPos_ = player_->GetTrans();

	dir_ = (player_->GetPosOfEnemyAttack() - beginPos_).GetNormalized();
	player_->SetFrontVec(dir_);

	CameraManager::GetInstance().GetCamera("playerCamera")->SetTarget(player_->GetPosOfEnemyAttack());
	CameraManager::GetInstance().GetCamera("playerCamera")->CameraShake(PUNCHED_CAMERA_SHAKE_TIME_, PUNCHED_CAMERA_SHAKE_LENGTH_);
}

void PlayerStateDeadEffectPunched::Update()
{
	t_ = (float)timer_ / player_->GetDeadEffectTimerMax();

	//ゆっくりにする
	GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(true);

	auto enemies = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "enemy");
	for (auto& enemy : enemies)
	{
		auto enemyl = dynamic_cast<Character*>(enemy);
		if (enemyl)
		{
			enemyl->SetAnimationSpeed(GameVelocityManager::GetInstance().GAME_VELOCITY_MIN_);
		}
	}

	//位置
	player_->SetTrans(LerpVec3(beginPos_, Vec3(beginPos_.x, -player_->GetScale().y, beginPos_.z) + Vec3(-dir_.x, 0, -dir_.z), EaseInOutBack(t_)));

	//正面ベクトルを変更
	player_->SetFrontVec((player_->GetFrontVec() + PUNCHED_FRONT_ADD_VEC_).GetNormalized());
	//仮で海用のカメラ角度
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = { player_->GetFrontVec().y,0,0 };
	//正面ベクトルを回転、そのベクトルをカメラのターゲットに使う
	auto camera = CameraManager::GetInstance().GetCamera("playerCamera");
	camera->SetTarget(player_->GetTrans() + player_->GetFrontVec());
	camera->SetEye(player_->GetTrans());

	//演出終わったら生存フラグオフ
	if (timer_ >= player_->GetDeadEffectTimerMax())
	{
		player_->SetIsAlive(false);
	}

	timer_++;
}
