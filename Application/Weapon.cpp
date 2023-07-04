#include "Weapon.h"
#include "GameVelocityManager.h"



void Weapon::NoParentMove()
{
	if (GetParent() == nullptr && GetWorldTrans().y_ > GetScale().x_)
	{
		//�N�[���^�C�����Q�[���X�s�[�h��������
		SetTrans(GetTrans() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		SetRot(GetRot() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		//���񂾂�キ
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

		//�n�ʂɂ�����
		if (GetWorldTrans().y_ <= GetScale().x_)
		{
			fallVec_ = { 0,0,0 };
		}
	}
}