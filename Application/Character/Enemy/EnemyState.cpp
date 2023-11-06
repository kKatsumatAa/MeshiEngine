#include "EnemyState.h"
#include "Enemy.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LevelManager.h"

//---------------
Vec3 EnemyStateAttackStance::ANGLE_MAX_ = { -0.3f,0.25f,-0.5f };


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
	//銃があれば
	if (enemy_->GetWeapon())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeaponAndMove>());
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
	enemy_->SetDissolveT(LerpVec3({ 1.0f,0,0 }, { 0,0,0 }, EaseIn(t)).x);

	if (timer_ >= EMERGE_TIMER_MAX_)
	{
		//ライトの使用オフ
		LightManager* lightM = LevelManager::GetInstance().GetLightManager();
		if (enemy_->GetLightIndexTmp() != enemy_->GetLightIndexInit())
		{
			lightM->SetPointLightActive(enemy_->GetLightIndexTmp(), false);
		}

		enemy_->SetColliderIsValid(true);

		//武器持ってるかどうかでステート変更
		ChangeState();
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
}

//---------------------------------------------------------------------------------------------
//武器持ってる状態の親ステート
void EnemyStateHaveWeapon::Initialize()
{
}

void EnemyStateHaveWeapon::Update()
{
	//親クラス処理
	EnemyState::Update();

	//武器失ったらステート変更
	if (enemy_->GetWeapon() == nullptr)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
	}

	//プレイヤーの方向にレイを飛ばして
	else if (enemy_->CheckRayOfEyeHit(
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
	//構え変更
	enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceBegin>());
}

void EnemyStateHaveWeaponAndAttack::Update()
{
	//アニメーションなど
	enemy_->SetIsPlayAnimation(false);
	enemy_->SetIsLoopAnimation(false);
	enemy_->DirectionUpdate(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	//親クラス処理
	EnemyStateHaveWeapon::Update();
}

//--------------------------------
//武器持ってて移動中
void EnemyStateHaveWeaponAndMove::Initialize()
{
	//構え戻す
	enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceEnd>());
}

void EnemyStateHaveWeaponAndMove::Update()
{
	enemy_->SetIsPlayAnimation(true);
	enemy_->SetIsLoopAnimation(true);
	enemy_->AllMove(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	//親クラス処理
	EnemyStateHaveWeapon::Update();
}

//-----------------------------------------------------------------------------------------
// 構え親クラス
void EnemyStateAttackStance::Update()
{
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	t_ = min(timer_ / TIMER_MAX_, 1.0f);
}

void EnemyStateAttackStance::DrawImgui()
{
	if (ImGui::TreeNode("StanceRot"))
	{
		ImGui::DragFloat3("rot", &ANGLE_MAX_.x);

		ImGui::TreePop();
	}
}

//------------------------
//攻撃構え始め
void EnemyStateAttackStanceBegin::Initialize()
{
	auto addRot = enemy_->GetNode(MOVE_NODE_NAME_)->addRot;

	stanceBeginRot_ = { addRot.x ,addRot.y,addRot.z };
	stanceEndRot_ = ANGLE_MAX_ - Vec3(addRot.x, addRot.y, addRot.z);
}

void EnemyStateAttackStanceBegin::Update()
{
	//親クラス処理
	EnemyStateAttackStance::Update();

	//角度を変える
	enemy_->SetNodeAddRot(MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, t_));

	//仮で構え終わったら攻撃
	if (t_ >= 1.0f)
	{
		enemy_->CheckRayOfEyeHit(
			(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
			Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_);

		//攻撃
		enemy_->Attack(info_.object->GetWorldTrans());
	}
}

//------------------------
//攻撃構え終わり
void EnemyStateAttackStanceEnd::Initialize()
{
	auto addRot = enemy_->GetNode(MOVE_NODE_NAME_)->addRot;

	stanceBeginRot_ = { addRot.x ,addRot.y,addRot.z };
	stanceEndRot_ = { 0, 0,0 };
}

void EnemyStateAttackStanceEnd::Update()
{
	//親クラス処理
	EnemyStateAttackStance::Update();

	//角度を戻す
	enemy_->SetNodeAddRot(MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, t_));
}


//被ダメージ始め---------------------------------------------------------------------------
void EnemyStateDamagedBegin::Initialize()
{
}

void EnemyStateDamagedBegin::Update()
{
	//時間を加算して割合を得る
	timer_ += GameVelocityManager::GetInstance().GetVelocity();
	float t = min(timer_ / TIMER_MAX_, 1.0f);

	//割合を使用して線形補完
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotBegin, nodeAddRot.addRotEnd, EaseOut(t)));
	}

	enemy_->HPUpdate(t);

	if (t >= 1.0f)
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

	//割合を使用して線形補完(今度は元に戻す)
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotEnd, nodeAddRot.addRotBegin, t));
	}

	enemy_->HPUpdate(t);

	if (t >= 1.0f)
	{
		ChangeState();
	}
}


//死亡時------------------------------------------------------------------------
void EnemyStateDead::Initialize()
{
	//死んだのでノードごとの当たり判定は無効に
	enemy_->SetIsValidNodeColliders(false);
}

void EnemyStateDead::Update()
{
	//時間を加算して割合を得る
	timer_ += GameVelocityManager::GetInstance().GetVelocity();
	float t = timer_ / enemy_->GetDeadTimerMax();

	//割合を使用して線形補完
	for (auto nodeAddRot : enemy_->GetDamagedAddRots())
	{
		enemy_->ObjectFBX::SetNodeAddRot(nodeAddRot.nodeName, LerpVec3(nodeAddRot.addRotBegin, nodeAddRot.addRotEnd, t));
	}

	enemy_->HPUpdate(t);

	//ノードの位置からパーティクルだす
	enemy_->DeadNodesParticle((uint64_t)(10.0f * t), (uint64_t)(Lerp((float)PARTICLE_INTERVAL_ * 2.0f, (float)PARTICLE_INTERVAL_, EaseIn(t))));

	//終わったら生きてるフラグオフ
	if (t >= 1.0f)
	{
		enemy_->SetIsAlive(false);
	}
}