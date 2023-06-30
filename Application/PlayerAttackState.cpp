#include "PlayerAttackState.h"
#include "CollisionManager.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"


//何もしない---------------------------------------------------------------
void PlayerAttackStateNone::Initialize()
{
}

void PlayerAttackStateNone::Update()
{
	//レイにプレイヤーの正面ベクトル入れる
	Ray ray;
	ray.dir = { player_->GetFrontVec().x_,player_->GetFrontVec().y_,player_->GetFrontVec().z_ };
	ray.start = { player_->GetTrans().x_,player_->GetTrans().y_,player_->GetTrans().z_ };

	RaycastHit info;
	//正面ベクトルに敵がいるか
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, &info, player_->GetAttackLength());
	if (isRayHit)
	{
		player_->SetIsCanAttack(true);

		//クリックしたら攻撃
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && info.collider->GetObject3d()->GetObjName() == "enemy")
		{
			CollisionInfo c = CollisionInfo(player_, player_->GetCollider(), info.inter);
			c.object_->SetObjName("playerAttack");
			info.collider->OnCollision(c);
			c.object_->SetObjName("player");

			//ゲームのスピード
			GameVelocityManager::GetInstance().AddGameVelocity(0.5f);
		}
	}
	else
	{
		player_->SetIsCanAttack(false);
	}
}

void PlayerAttackStateNone::Draw()
{
}


//攻撃1---------------------------------------------------------------
void PlayerAttackStateDoing::Initialize()
{
}

void PlayerAttackStateDoing::Update()
{
}

void PlayerAttackStateDoing::Draw()
{
}


//攻撃2---------------------------------------------------------------
void PlayerAttackStateDoing2::Initialize()
{
}

void PlayerAttackStateDoing2::Update()
{
}

void PlayerAttackStateDoing2::Draw()
{
}
