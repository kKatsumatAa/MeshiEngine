#include "Weapon.h"
#include "GameVelocityManager.h"



void Weapon::NoParentMove()
{
	if (GetParent() == nullptr && GetWorldTrans().y_ > GetScale().x_)
	{
		//クールタイムもゲームスピードをかける
		SetTrans(GetTrans() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		SetRot(GetRot() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		//だんだん弱く
		fallVec_.x_ *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));


		if (fallVec_.y_ < 0.05f && fallVec_.y_ > -1.5f)
		{
			fallVec_.y_ = -(fabsf(fallVec_.y_) + fabsf(fallVec_.y_) * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));
		}
		else if (fallVec_.y_ >= 0.05f)
		{
			fallVec_.y_ *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));
		}

		fallVec_.z_ *= (0.9f + 0.1f * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));

		//地面についたら
		if (GetWorldTrans().y_ <= GetScale().x_)
		{
			fallVec_ = { 0,0,0 };
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

	Update();
}
