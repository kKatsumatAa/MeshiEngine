#include "PlayerHandManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"
#include "CollisionAttribute.h"
#include "ObjectManager.h"
#include "CameraManager.h"



void PlayerHandManager::Initialize(Player* player)
{
	player_ = player;

	//位置(ローカル座標をセット)
	Vec3 playerPos = player->GetTrans();
	Vec3 distance = { 1.0f,1.0f,1.0f };
	//生成（仮）
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { -distance.x * 2.0f,0 - distance.y,distance.z }, true, HAND_R_NAME_)));
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { +distance.x * 2.0f,0 - distance.y,distance.z }, false, HAND_L_NAME_)));

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
		hand->ChangeAttackState(std::make_unique<PlayerAttackStateDoing>());

		//ステート内で呼び出す
		std::function<void(PlayerHand*)>f = [=](PlayerHand* playerHand) {

			//敵がもう倒されてる可能性があるので
			if (info.collider)
			{
				//当たり判定呼び出し処理で、プレイヤーの位置等を使い、敵に攻撃被弾時の処理をさせるため
				c_ = CollisionInfo(playerHand, playerHand->GetCollider(), info.inter);
				info.collider->OnCollision(c_);

				//カメラシェイク
				CameraManager::GetInstance().GetCamera("playerCamera")->CameraShake(5, 1.05f);

				//一回当たったらもう呼び出さないようにするため
				playerHand->SetColliderIsValid(false);
			}
		};

		//敵の被弾処理セット
		hand->GetAttackState()->SetEnemyDamageFunc(f);

		//ゲームのスピード(仮)
		GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
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
