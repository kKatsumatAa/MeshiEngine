#include"EnemyStanceState.h"
#include"EnemyState.h"
#include"Enemy.h"
#include"GameVelocityManager.h"


//Imguiの角度のドラッグスピード
const float EnemyStateAttackStance::ANGLE_IMGUI_DRAG_SPEED_ = 0.05f;

//----------------------------------------------------------------------------
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

std::string EnemyStateAttackStance::GetHaveWeaponLeftRight()
{
	std::string leftRight = "Right";

	if (enemy_->WEAPON_PARENT_NODE_NAME_.find("Left") != std::string::npos)
	{
		leftRight = "Left";
	}

	return std::string(leftRight);
}

//------------------------
//銃の攻撃構え始め
void EnemyStateAttackStanceBegin::Initialize()
{
	auto nowAddRot = enemy_->GetNode(GetHaveWeaponLeftRight() + MOVE_NODE_NAME_)->addRot;
	stanceBeginRot_ = Vec3(nowAddRot.x, nowAddRot.y, nowAddRot.z);
}

void EnemyStateAttackStanceBegin::Update()
{
	//親クラス処理
	EnemyStateAttackStance::Update();

	//今の角度から構えの角度を出す
	auto nowNodeRot = enemy_->GetNode(GetHaveWeaponLeftRight() + MOVE_NODE_NAME_)->rotation;
	stanceEndRot_ = ANGLE_MAX_ - Vec3(nowNodeRot.x, nowNodeRot.y, nowNodeRot.z);

	//角度を変える
	enemy_->SetNodeAddRot(GetHaveWeaponLeftRight() + MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, timeRatio_));

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
//銃の攻撃構え終わり
void EnemyStateAttackStanceEnd::Initialize()
{
	auto nowRot = enemy_->GetNode(GetHaveWeaponLeftRight() + MOVE_NODE_NAME_)->addRot;

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
	enemy_->SetNodeAddRot(GetHaveWeaponLeftRight() + MOVE_NODE_NAME_, LerpVec3(stanceBeginRot_, stanceEndRot_, timeRatio_));
}

//------------------------------------------------
//素手攻撃何もない
void EnemyStateBareHandsAttackNone::Initialize()
{
	enemy_->SetIsBareAttack(false);
	//歩き
	enemy_->PlayAnimation(true, enemy_->GetWalkAnimNum(), false);
}

void EnemyStateBareHandsAttackNone::Update()
{
}

//素手攻撃始め
void EnemyStateBareHandsAttackBegin::Initialize()
{
	//素手殴りフラグ
	enemy_->SetIsBareAttack(true);

	//歩くのをやめる
	enemy_->SetIsPlayAnimation(false, enemy_->GetWalkAnimNum());
}

void EnemyStateBareHandsAttackBegin::Update()
{
	timer_ += GameVelocityManager::GetInstance().GetVelocity();

	t_ = min(timer_, TIME_) / TIME_;

	//アニメーションをブレンド（歩き→殴る）
	enemy_->BlendAnimationUpdate(enemy_->GetWalkAnimNum(), Enemy::AnimationNum::PUNCH, t_);

	enemy_->DirectionUpdate(GetRayHitGunOrPlayerPos());

	if (timer_ >= TIME_)
	{
		enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateBareHandsAttack>());
	}
}

//------------
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
		enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateBareHandsAttackEnd>());
	}
}

//---------------
//素手の攻撃終わり
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
	enemy_->BlendAnimationUpdate(Enemy::AnimationNum::PUNCH, enemy_->GetWalkAnimNum(), timeRatio_);

	if (timer_ >= TIME_MAX_)
	{
		enemy_->ChangeEnemyStanceState(std::make_unique<EnemyStateBareHandsAttackNone>());
	}
}