#include "Weapon.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"
#include "ParticleManager.h"
#include "Character.h"


void Weapon::NoParentMove()
{
	if (GetParent() == nullptr)
	{
		//前回の位置
		oldPos_ = GetTrans();

		//クールタイムもゲームスピードをかける
		SetTrans(GetTrans() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		SetRot(GetRot() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		//だんだん弱く
		fallVec_.x *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));


		if (fallVec_.y < 0.1f && fallVec_.y > FALL_VEC_Y_MIN_)
		{
			fallVec_.y = -(fabsf(fallVec_.y) + fabsf(fallVec_.y) * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));
		}
		else if (fallVec_.y >= 0.1f)
		{
			fallVec_.y *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));
		}

		//だんだん弱く
		fallVec_.z *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));


		//前回の位置から今の位置のベクトルをレイとして判定
		{
			RaycastHit info;

			//敵と地形と判定をとる
			if (CollisionManager::GetInstance()->RaycastUtil(GetTrans() + fallVec_ * GetScale().GetLength(), oldPos_, COLLISION_ATTR_LANDSHAPE | COLLISION_ATTR_ENEMYS, &info))
			{
				info.collider->OnCollision(CollisionInfo(this, GetCollider(), info.inter));
				OnLandShape({ info.inter.m128_f32[0],info.inter.m128_f32[1], info.inter.m128_f32[2] });
			}
		}
	}
}

void Weapon::ChangeOwner(Object* parent)
{
	if (parent == nullptr)
	{
		//離れた瞬間にワールド座標を入れる(親がいなくなるので)
		SetTrans(GetWorldTrans());
		//親をnull
		SetParent((WorldMat*)nullptr);
	}
	else
	{
		//拾われ、親がはいったらローカル座標に切り替え
		SetTrans(localPos_);
		//親設定
		SetParent(parent->GetWorldMat());
	}
	//所有者も設定
	owner_ = parent;
}

void Weapon::Update()
{
	//消えたら親を解除
	if (!GetIsAlive() && owner_)
	{
		Character* chara = dynamic_cast<Character*>(owner_);
		owner_ = nullptr;
		chara->FallWeapon({ 0,0,0 });
	}

	Object::Update();
}
