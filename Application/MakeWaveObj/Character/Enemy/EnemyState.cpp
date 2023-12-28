#include "EnemyState.h"
#include "Enemy.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LevelManager.h"

using namespace Constant;

//---------------
Vec3 EnemyStateAttackStance::ANGLE_MAX_ = { 0.25f,0.37f,-0.55f };


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
	//素手で殴ってたら素手殴りの終わりに遷移
	if (enemy_->GetIsBareAttack())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHandsAttackEnd>());
	}
	//銃があれば
	else if (enemy_->GetWeapon())
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
	//歩き
	enemy_->PlayAnimation(true, Enemy::AnimationNum::WALK, false);
}

void EnemyStateBareHands::Update()
{
	enemy_->AllMove(GetRayHitGunOrPlayerPos());

	EnemyState::Update();

	//素手で一定範囲にplayerがいたら
	if (enemy_->GetPlayerIsWithinRange())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHandsAttackBegin>());
	}
}

//------------------------------------------------
//素手攻撃始め
void EnemyStateBareHandsAttackBegin::Initialize()
{
	//素手殴りフラグ
	enemy_->SetIsBareAttack(true);

	//歩くのをやめる
	enemy_->SetIsPlayAnimation(false, Enemy::AnimationNum::WALK);
}

void EnemyStateBareHandsAttackBegin::Update()
{
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	t_ = min(timer_, TIME_) / TIME_;

	//アニメーションをブレンド（歩き→殴る）
	enemy_->BlendAnimationUpdate(Enemy::AnimationNum::WALK, Enemy::AnimationNum::PUNCH, t_);

	enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());

	if (timer_ >= TIME_)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHandsAttack>());
	}
}

//------------------------------------------------
//素手で攻撃中
void EnemyStateBareHandsAttack::Initialize()
{
	//殴るの開始
	enemy_->PlayAnimation(false, Enemy::AnimationNum::PUNCH);
}

void EnemyStateBareHandsAttack::Update()
{
	timer_ += enemy_->GetAnimeSpeedExtend();

	enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());

	if (!enemy_->GetAnimData(Enemy::AnimationNum::PUNCH).isPlay_)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHandsAttackEnd>());
	}
}

//-------------------------------------------------
//素手で攻撃終わり
void EnemyStateBareHandsAttackEnd::Initialize()
{
	//殴るのおわり
	enemy_->SetIsPlayAnimation(false, Enemy::AnimationNum::PUNCH);
}

void EnemyStateBareHandsAttackEnd::Update()
{
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	timeRatio_ = min(timer_, TIME_MAX_) / TIME_MAX_;

	enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());

	//アニメーションをブレンド（殴る→歩き）
	enemy_->BlendAnimationUpdate(Enemy::AnimationNum::PUNCH, Enemy::AnimationNum::WALK, timeRatio_);

	if (timer_ >= TIME_MAX_)
	{
		enemy_->SetIsBareAttack(false);

		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
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

//-------------------------------------------------------------------------------------------------------
//Imguiの角度のドラッグスピード
const float EnemyStateAttackStance::ANGLE_IMGUI_DRAG_SPEED_ = 0.05f;
// 構え親クラス
void EnemyStateAttackStance::Update()
{
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	timeRatio_ = min(timer_ / TIMER_MAX_, TIME_RATIO_MAX_);
}

void EnemyStateAttackStance::DrawImgui()
{
	if (ImGui::TreeNode("StanceRot"))
	{
		ImGui::DragFloat3("rot", &ANGLE_MAX_.x, ANGLE_IMGUI_DRAG_SPEED_);

		ImGui::TreePop();
	}
}

//------------------------
//攻撃構え始め
void EnemyStateAttackStanceBegin::Initialize()
{
	auto nowAddRot = enemy_->GetNode(MOVE_NODE_NAME_)->addRot;
	stanceBeginRot_ = Vec3(nowAddRot.x, nowAddRot.y, nowAddRot.z);
}

void EnemyStateAttackStanceBegin::Update()
{
	//親クラス処理
	EnemyStateAttackStance::Update();

	//今の角度から構えの角度を出す
	auto nowNodeRot = enemy_->GetNode(MOVE_NODE_NAME_)->rotation;
	stanceEndRot_ = ANGLE_MAX_ - Vec3(nowNodeRot.x, nowNodeRot.y, nowNodeRot.z);

	//角度を変える
	enemy_->SetNodeAddRot(MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, timeRatio_));

	//アニメーションスピード徐々に
	enemy_->SetAnimeSpeedExtend(Lerp(ObjectFBX::NORMAL_ANIM_SPEED_, 0, EaseIn(timeRatio_)));

	//仮で構え終わったら攻撃
	if (timeRatio_ >= TIME_RATIO_MAX_)
	{
		enemy_->CheckRayOfEyeHit(
			(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
			Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_);

		//攻撃
		if (info_.object)
		{
			enemy_->Attack(info_.object->GetWorldTrans());
		}
	}
}

//------------------------
//攻撃構え終わり
void EnemyStateAttackStanceEnd::Initialize()
{
	auto nowRot = enemy_->GetNode(MOVE_NODE_NAME_)->addRot;

	stanceBeginRot_ = { nowRot.x ,nowRot.y,nowRot.z };
	stanceEndRot_ = { 0,0,0 };
}

void EnemyStateAttackStanceEnd::Update()
{
	//親クラス処理
	EnemyStateAttackStance::Update();

	//アニメーションスピード徐々に
	enemy_->SetAnimeSpeedExtend(Lerp(0, ObjectFBX::NORMAL_ANIM_SPEED_, EaseIn(timeRatio_)));

	//角度を戻す
	enemy_->SetNodeAddRot(MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, timeRatio_));
}


//被ダメージ始め---------------------------------------------------------------------------
void EnemyStateDamagedBegin::Initialize()
{
	//殴るアニメの停止
	enemy_->SetIsPlayAnimation(false, Enemy::AnimationNum::PUNCH);
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

	//ノードの位置からパーティクルだす(徐々に数減らしたり)
	enemy_->DeadNodesParticle((uint64_t)(DEAD_PARTICLE_NUM_ * t), (uint64_t)(Lerp((float)PARTICLE_INTERVAL_ * TWICE, (float)PARTICLE_INTERVAL_, EaseIn(t))));

	//終わったら生きてるフラグオフ
	if (t >= TIME_RATIO_MAX_)
	{
		enemy_->SetIsAlive(false);
	}
}