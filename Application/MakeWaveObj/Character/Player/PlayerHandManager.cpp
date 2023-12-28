#include "PlayerHandManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerHandState.h"
#include "CollisionAttribute.h"
#include "ObjectManager.h"
#include "CameraManager.h"



void PlayerHandManager::Initialize(Player* player)
{
	player_ = player;

	//位置(ローカル座標をセット)
	//生成（仮）
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { -LOCAL_POS_TMP_.x,LOCAL_POS_TMP_.y,LOCAL_POS_TMP_.z }, true, HAND_R_NAME_)));
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { LOCAL_POS_TMP_.x,LOCAL_POS_TMP_.y,LOCAL_POS_TMP_.z }, false, HAND_L_NAME_)));

	handL_ = dynamic_cast<PlayerHand*>(*ObjectManager::GetInstance().GetObjs(OBJ_GROUP_NAME_, HAND_L_NAME_).begin());
	handR_ = dynamic_cast<PlayerHand*>(*ObjectManager::GetInstance().GetObjs(OBJ_GROUP_NAME_, HAND_R_NAME_).begin());
}

void PlayerHandManager::HandAttack(PlayerHand* hand, const RaycastHit& info)
{
	if (hand == nullptr)
	{
		return;
	}

	//当たった相手が敵
	if (info.collider->GetAttribute() & COLLISION_ATTR_ENEMYS && player_)
	{
		//衝突点までの距離
		Vec3 lengthV = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - player_->GetTrans();
		hand->SetInterLength(lengthV.GetLength());

		//攻撃可能に変更
		hand->SetColliderIsValid(true);

		//ステート変更
		hand->ChangeAttackState(std::make_unique<PlayerHandStateDoing>());

		//ステート内で呼び出す
		std::function<void(PlayerHand*)>f = [=](PlayerHand* playerHand) {

			//敵がもう倒されてる可能性があるので
			if (info.collider)
			{
				//当たり判定呼び出し処理で、プレイヤーの位置等を使い、敵に攻撃被弾時の処理をさせるため
				c_ = CollisionInfo(playerHand, nullptr, info.inter);
				info.collider->OnCollision(c_);

				//カメラシェイク
				CameraManager::GetInstance().GetCamera("playerCamera")->CameraShake(PUNCH_CAMERA_SHAKE_TIME_, PUNCH_CAMERA_SHAKE_LENGTH_);

				//一回当たったらもう呼び出さないようにするため
				playerHand->SetColliderIsValid(false);
			}
			};

		//敵の被弾処理セット
		hand->GetAttackState()->SetEnemyDamageFunc(f);

		//ゲームのスピード足す(仮)
		GameVelocityManager::GetInstance().AddGameVelocity(GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_);
	}
}

bool PlayerHandManager::GetIsUseWitchHand()
{
	//手のどちらかが使えたら攻撃
	if (!handR_->GetIsAttacking() || !handL_->GetIsAttacking())
	{
		return true;
	}

	return false;
}

PlayerHand* PlayerHandManager::GetWitchUseHand()
{
	if (!handR_->GetIsAttacking())
	{
		return handR_;
	}
	if (!handL_->GetIsAttacking())
	{
		return handL_;
	}

	return nullptr;
}

void PlayerHandManager::Attack(RaycastHit info)
{
	//手のどちらかが使えたら攻撃
	if (GetIsUseWitchHand())
	{
		//攻撃
		HandAttack(GetWitchUseHand(), info);
	}
}

void PlayerHandManager::Update()
{
}

void PlayerHandManager::Draw()
{
}

void PlayerHandManager::DeleteHands()
{
	ObjectManager::GetInstance().ClearGroup(OBJ_GROUP_NAME_);
}
