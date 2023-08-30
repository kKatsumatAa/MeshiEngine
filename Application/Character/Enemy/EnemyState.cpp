#include "EnemyState.h"
#include "Enemy.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LevelManager.h"



bool EnemyState::CheckEyeRayHit()
{
	bool isRayHit = enemy_->CheckRayOfEyeHit(enemy_->GetFrontVec(),
		Enemy::S_LENGTH_MAX_, COLLISION_ATTR_ALLIES, &info_);

	return isRayHit;
}

Vec3 EnemyState::GetRayHitGunOrPlayerPos()
{
	//銃
	auto guns = ObjectManager::GetInstance().GetObjs("gun");

	float minLength = Enemy::S_LENGTH_MAX_;
	bool gunSee = false;
	Vec3 gunPos;

	//全ての銃に向かってレイを飛ばし、障害物がなければ
	for (auto gun : guns)
	{
		if (enemy_->CheckRayOfEyeHit((gun->GetWorldTrans() - enemy_->GetWorldTrans()).GetNormalized(),
			Enemy::S_LENGTH_MAX_, BARE_HANDS_ATTR_TMP_, &info_))
		{
			//銃で所有者がいなければ
			if (info_.object->GetObjName() == "gun" && info_.object->GetParent() == nullptr)
			{
				//距離
				float distance = (info_.object->GetTrans() - enemy_->GetWorldTrans()).GetLength();

				//一番近ければ保存
				if (minLength > distance)
				{
					minLength = distance;
					gunPos = info_.object->GetTrans();
				}
				gunSee = true;
			}
		}
	}

	//銃が視界にあれば
	if (gunSee)
	{
		return gunPos;
	}

	//なければプレイヤーの
	return CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
}

void EnemyState::Update()
{
	enemy_->HPUpdate();
}


//出現演出-----------------------------------------------------------------------
void EnemyStateEmergeEffect::Initialize()
{
}

void EnemyStateEmergeEffect::Update()
{
	enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());
	//当たり判定
	enemy_->SetColliderIsValid(false);
	//壁と床との判定
	enemy_->CollisionWallAndFloor();

	//ディゾルブ
	float t = timer_ / EMERGE_TIMER_MAX_;
	enemy_->SetDissolveT(LerpVec3({ 1.0f,0,0 }, { 0,0,0 }, EaseIn(t)).x_);

	if (timer_ >= EMERGE_TIMER_MAX_)
	{
		//ライトの使用オフ
		LightManager* lightM = LevelManager::GetInstance().GetLightManager();
		if (enemy_->GetLightIndexTmp() != enemy_->GetLightIndexInit())
		{
			lightM->SetPointLightActive(enemy_->GetLightIndexTmp(), false);
		}

		enemy_->SetColliderIsValid(true);

		//銃があれば
		if (enemy_->GetWeapon())
		{
			enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeapon>());
		}
		else
		{
			enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
		}
	}

	timer_++;
}



//素手状態-----------------------------------------------------------------------
void EnemyStateBareHands::Initialize()
{
	enemy_->SetIsPlayAnimation(true);
	enemy_->SetIsLoopAnimation(true);
}

void EnemyStateBareHands::Update()
{
	enemy_->AllMove(GetRayHitGunOrPlayerPos());

	EnemyState::Update();

	//武器持ったらステート変更
	if (enemy_->GetWeapon())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeapon>());
	}
}


//武器持ってる状態-----------------------------------------------------------------------
void EnemyStateHaveWeapon::Initialize()
{
}

void EnemyStateHaveWeapon::Update()
{
	//プレイヤーの方向にレイを飛ばして
	if (enemy_->CheckRayOfEyeHit(
		(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
		Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_)
		)
	{
		//プレイヤーが見えたら動かず攻撃
		if (info_.object->GetObjName() == "player")
		{
			enemy_->SetIsPlayAnimation(false);
			enemy_->SetIsLoopAnimation(false);
			enemy_->Attack(info_.object->GetWorldTrans());
			enemy_->DirectionUpdate(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());
		}
		//仮）見えなければプレイヤーの方向に移動
		else
		{
			enemy_->SetIsPlayAnimation(true);
			enemy_->SetIsLoopAnimation(true);
			enemy_->AllMove(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());
		}
	}

	EnemyState::Update();

	//武器失ったらステート変更
	if (enemy_->GetWeapon() == nullptr)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
	}
}
