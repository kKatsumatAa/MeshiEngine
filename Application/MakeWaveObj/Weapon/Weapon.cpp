#include "Weapon.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"
#include "ParticleManager.h"
#include "Character.h"


Weapon::~Weapon()
{
	TerminateParent();
}

void Weapon::NoParentMove()
{
	if (GetParent() == nullptr && isThrowing_)
	{
		//前回の位置
		oldPos_ = GetTrans();

		const float FRAME_VEL_EXTEND_REM = FRAME_VEL_RATIO_MAX_ - FRAME_VEL_EXTEND_;

		//ゲームスピードをかける
		float frameVelExtend = powf(GameVelocityManager::GetInstance().GetVelocity(), GAME_VEL_POW_);
		velocity_ = fallVec_ * frameVelExtend;

		SetTrans(GetTrans() + velocity_);

		SetRot(GetRot() + fallVec_ * frameVelExtend);

		//だんだん弱く
		fallVec_.x *= (FRAME_VEL_EXTEND_ + FRAME_VEL_EXTEND_REM * (FRAME_VEL_RATIO_MAX_ - frameVelExtend));

		//重力
		float gravity = GRAVITY_TMP_ * frameVelExtend;
		fallVec_.y = max(fallVec_.y - gravity, -GRAVITY_MAX_);

		//だんだん弱く
		fallVec_.z *= (FRAME_VEL_EXTEND_ + FRAME_VEL_EXTEND_REM * (FRAME_VEL_RATIO_MAX_ - frameVelExtend));


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

void Weapon::ChangeOwner(IObject3D* parent)
{
	if (parent == nullptr)
	{
		//離れた瞬間にワールド座標を入れる(親がいなくなるので)
		SetTrans(GetWorldTrans());
		//親をnull
		SetParent((WorldMat*)nullptr);
		isThrowing_ = true;
	}
	else
	{
		//拾われ、親がはいったらローカル座標に切り替え
		SetTrans(localPos_);
		//親設定
		SetParent(parent->GetWorldMat());
		isThrowing_ = false;
	}
	//所有者も設定
	owner_ = parent;
	fallVec_.y = 0;
}

void Weapon::SetAttribute(uint16_t attribute)
{
	if (collider_)
	{
		GetCollider()->SetAttribute(attribute);
	}
}

void Weapon::Update()
{
	Object::Update();
}

void Weapon::DrawShadow()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::SHADOW_OBJ);
}

void Weapon::Draw()
{
	Object::DrawModel(nullptr);
}

void Weapon::TerminateParent()
{
	if (owner_)
	{
		Character* chara = dynamic_cast<Character*>(owner_);
		owner_ = nullptr;
		chara->FallHaveWeapon({ 0,0,0 });
	}
}
