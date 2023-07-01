#include "PlayerHandManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"



void PlayerHandManager::Initialize(Player* player)
{
	handL.reset();
	handR.reset();

	//位置(ローカル座標をセット)
	Vec3 playerPos = player->GetTrans();
	Vec3 distance = player->GetScale();
	//生成（仮）
	handL = handL->Create(player, { -distance.x_ * 2.0f,0 - distance.y_,-distance.z_ });
	handR = handR->Create(player, { +distance.x_ * 2.0f,0 - distance.y_,-distance.z_ });

	player_ = player;
}

void PlayerHandManager::HandAttack(PlayerHand* hand, const RaycastHit& info)
{
	if (hand == nullptr)
	{
		return;
	}

	//クリックしたら攻撃
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && info.collider->GetObject3d()->GetObjName() == "enemy")
	{
		//衝突点までの距離
		Vec3 lengthV = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - player_->GetTrans();
		hand->SetInterLength(lengthV.GetLength());

		//ステート変更
		hand->ChangeAttackState(std::make_unique<PlayerAttackStateDoing>());

		//ステート内で呼び出す
		std::function<void()>f = [=]() {
			//当たり判定呼び出し処理で、プレイヤーの位置等を使い、敵に攻撃被弾時の処理をさせるため
			CollisionInfo c = CollisionInfo(player_, player_->GetCollider(), info.inter);
			c.object_->SetObjName("playerAttack");
			info.collider->OnCollision(c);
			c.object_->SetObjName("player");
		};
		//敵の被弾処理セット
		hand->GetAttackState()->SetEnemyDamageFunc(f);

		//ゲームのスピード(仮)
		GameVelocityManager::GetInstance().AddGameVelocity(0.5f);
	}
}

bool PlayerHandManager::GetIsUseWitchHand()
{
	//手のどちらかが使えたら攻撃
	if (!handR->GetIsAttacking() || !handL->GetIsAttacking())
	{
		return true;
	}

	return false;
}

PlayerHand* PlayerHandManager::GetWitchUseHand()
{
	if (!handR->GetIsAttacking())
	{
		return handR.get();
	}
	if (!handL->GetIsAttacking())
	{
		return handL.get();
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
	//手
	handL->Update();
	handR->Update();
}

void PlayerHandManager::Draw()
{
	handL->Draw();
	handR->Draw();
}
