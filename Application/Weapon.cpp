#include "Weapon.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"
#include "ParticleManager.h"


void Weapon::NoParentMove()
{
	if (GetParent() == nullptr)
	{
		//�O��̈ʒu
		oldPos_ = GetTrans();

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


		//�O��̈ʒu���獡�̈ʒu�̃x�N�g�������C�Ƃ��Ĕ���
		{
			RaycastHit info;

			if (CollisionManager::GetInstance()->RaycastUtil(GetTrans(), oldPos_, COLLISION_ATTR_LANDSHAPE, &info))
			{
				OnLandShape({ info.inter.m128_f32[0],info.inter.m128_f32[1], info.inter.m128_f32[2] });
			}
		}
	}
}

void Weapon::ChangeOwner(Object* parent)
{
	if (parent == nullptr)
	{
		//���ꂽ�u�ԂɃ��[���h���W������(�e�����Ȃ��Ȃ�̂�)
		SetTrans(GetWorldTrans());
		//�e��null
		SetParent((WorldMat*)nullptr);
	}
	else
	{
		//�E���A�e���͂������烍�[�J�����W�ɐ؂�ւ�
		SetTrans(localPos_);
		//�e�ݒ�
		SetParent(parent->GetWorldMat());
	}
	//���L�҂��ݒ�
	owner_ = parent;

	Update();
}
