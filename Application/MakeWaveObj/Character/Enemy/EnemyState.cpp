#include "EnemyState.h"
#include "EnemyStanceState.h"
#include "Enemy.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LevelManager.h"

using namespace Constant;

//---------------
Vec3 EnemyStateAttackStance::ANGLE_MAX_ = { 0.55f,-0.42f,0.25f };


bool EnemyState::CheckEyeRayHit()
{
	bool isRayHit = enemy_->CheckRayOfEyeHit(enemy_->GetFrontVec(),
		Enemy::S_LENGTH_MAX_, COLLISION_ATTR_ALLIES, &info_);

	return isRayHit;
}

Vec3 EnemyState::GetRayHitGunOrPlayerPos()
{
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

bool EnemyState::GetPlayerVisually()
{
	if (enemy_->CheckRayOfEyeHit(
		(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
		Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_)
		)
	{
		//プレイヤーが見えたら
		if (info_.object->GetObjName() == "player")
		{
			return true;
		}
	}

	return false;
}

void EnemyState::ChangeState()
{
	//銃、頭があれば
	if (enemy_->GetWeapon() && enemy_->GetIsHavingHead())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeapon>());
	}
	else
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
	}
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
	enemy_->SetDissolveRatio(Lerp(IObject3D::DISSOLVE_RATE_MAX_, 0, EaseIn(t)));

	//ライトの色
	enemy_->SetEmergeLight(EaseIn(t));

	if (timer_ >= EMERGE_TIMER_MAX_)
	{
		//ライトの使用オフ
		enemy_->InactiveEmergeLight();

		enemy_->SetColliderIsValid(true);

		//武器持ってるかどうかでステート変更
		ChangeState();
	}

	timer_++;
}



//素手状態-----------------------------------------------------------------------
void EnemyStateBareHands::Initialize()
{
}

void EnemyStateBareHands::Update()
{
	//素手攻撃中は動かないように
	if (!enemy_->GetIsBareAttack())
	{
		enemy_->AllMove(GetRayHitGunOrPlayerPos());
	}

	EnemyState::Update();

	//素手で一定範囲にplayerがいて、頭があれば
	if (enemy_->GetPlayerIsWithinRange() && !enemy_->GetIsBareAttack() && enemy_->GetIsHavingHead())
	{
		enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateBareHandsAttackBegin>());
	}
}

//---------------------------------------------------------------------------------------------
//武器持ってる状態の親ステート
void EnemyStateHaveWeapon::Initialize()
{
	enemy_->SetIsPlayAnimation(true);
	enemy_->SetIsLoopAnimation(true);
}

void EnemyStateHaveWeapon::Update()
{
	//親クラス処理
	EnemyState::Update();

	//アニメーションなど
	enemy_->AllMove(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	//プレイヤーの方向にレイを飛ばして
	if (enemy_->CheckRayOfEyeHit(
		(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
		Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_))
	{
		//プレイヤーが見えたら動かず攻撃
		if (info_.object->GetObjName() == "player" && !enemy_->GetIsAttacking())
		{
			enemy_->SetIsAttacking(true);
			enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeaponAndAttack>());
		}
		//仮）見えなければプレイヤーの方向に移動
		else if (info_.object->GetObjName() != "player" && enemy_->GetIsAttacking())
		{
			enemy_->SetIsAttacking(false);
			enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeaponAndMove>());
		}
	}
}

//-------------------------------
//武器持ってて攻撃中
void EnemyStateHaveWeaponAndAttack::Initialize()
{
	EnemyStateHaveWeapon::Initialize();
	//構え変更
	enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceBegin>());
}

void EnemyStateHaveWeaponAndAttack::Update()
{
	//親クラス処理
	EnemyStateHaveWeapon::Update();
}

//--------------------------------
//武器持ってて移動中
void EnemyStateHaveWeaponAndMove::Initialize()
{
	EnemyStateHaveWeapon::Initialize();
	//構え戻す
	enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceEnd>());
}

void EnemyStateHaveWeaponAndMove::Update()
{
	//親クラス処理
	EnemyStateHaveWeapon::Update();
}

//被ダメージ始め---------------------------------------------------------------------------
void EnemyStateDamagedBegin::Initialize()
{
	//殴るアニメの停止
	if (enemy_->GetIsBareAttack())
	{
		enemy_->SetIsPlayAnimation(false, Enemy::AnimationNum::PUNCH);
		enemy_->AnimationReset(Enemy::AnimationNum::PUNCH);
		enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateBareHandsAttackEnd>());
	}
	else
	{
		//歩きのアニメーション(crawlと切り替える)
		enemy_->PlayAnimation(true, enemy_->GetWalkAnimNum(), false);
	}
	//攻撃フラグオフ
	enemy_->SetIsAttacking(false);
}

void EnemyStateDamagedBegin::Update()
{
	//時間を加算して割合を得る
	timer_ += GameVelocityManager::GetInstance().GetVelocity();
	float t = min(timer_ / TIMER_MAX_, TIME_RATIO_MAX_);

	//殴ってる途中だったら歩かせない
	if (enemy_->GetIsBareAttack())
	{
		enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());
	}
	else
	{
		enemy_->AllMove(GetRayHitGunOrPlayerPos());
	}
	//アニメーションスピード徐々に
	enemy_->SetAnimeSpeedExtend(Lerp(ObjectFBX::NORMAL_ANIM_SPEED_, DAMAGED_ANIME_SPEED_RATE_, EaseIn(t)));

	//割合を使用して線形補完
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotBegin, nodeAddRot.addRotEnd, EaseOut(t)));
	}

	enemy_->HPUpdate(t);

	if (t >= TIME_RATIO_MAX_)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateDamagedEnd>());
	}
}

//被ダメージ終わり---------------------------------------------------------------------------
void EnemyStateDamagedEnd::Initialize()
{
}

void EnemyStateDamagedEnd::Update()
{
	//時間を加算して割合を得る
	timer_ += GameVelocityManager::GetInstance().GetVelocity();
	float t = timer_ / TIMER_MAX_;

	//殴ってる途中だったら歩かせない
	if (enemy_->GetIsBareAttack())
	{
		enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());
	}
	else
	{
		enemy_->AllMove(GetRayHitGunOrPlayerPos());
	}

	//アニメーションスピード徐々に
	enemy_->SetAnimeSpeedExtend(Lerp(DAMAGED_ANIME_SPEED_RATE_, ObjectFBX::NORMAL_ANIM_SPEED_, EaseIn(t)));

	//割合を使用して線形補完(今度は元に戻す)
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotEnd, nodeAddRot.addRotBegin, t));
	}

	enemy_->HPUpdate(t);

	if (t >= TIME_RATIO_MAX_)
	{
		ChangeState();
	}
}

//死亡時------------------------------------------------------------------------
void EnemyStateDead::Initialize()
{
	//死んだのでノードごとの当たり判定は無効に
	enemy_->SetIsValidNodeColliders(false);
	enemy_->SetIsDead(true);
}

void EnemyStateDead::Update()
{
	//時間を加算して割合を得る
	timer_ += GameVelocityManager::GetInstance().GetVelocity();
	float t = timer_ / enemy_->GetDeadEffectTimerMax();

	//アニメーションスピード徐々に
	enemy_->AllMove(GetRayHitGunOrPlayerPos(), false);
	enemy_->SetAnimeSpeedExtend(Lerp(ObjectFBX::NORMAL_ANIM_SPEED_, 0, EaseIn(t)));

	//割合を使用して線形補完
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotBegin, nodeAddRot.addRotEnd, t));
	}

	enemy_->HPUpdate(t);

	//終わったら生きてるフラグオフ
	if (t >= TIME_RATIO_MAX_)
	{
		enemy_->SetIsAlive(false);
	}
}