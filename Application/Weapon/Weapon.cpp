#include "Weapon.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"
#include "ParticleManager.h"
#include "Character.h"


void Weapon::NoParentMove()
{
	if (GetParent() == nullptr)
	{
		//�O��̈ʒu
		oldPos_ = GetTrans();

		const float FRAME_VEL_EXTEND_REM = 1.0f - FRAME_VEL_EXTEND_;

		//�N�[���^�C�����Q�[���X�s�[�h��������
		SetTrans(GetTrans() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		SetRot(GetRot() + fallVec_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2));

		//���񂾂�キ
		fallVec_.x *= (FRAME_VEL_EXTEND_ + FRAME_VEL_EXTEND_REM * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));

		//�d��
		float gravity = GRAVITY_TMP_ * powf(GameVelocityManager::GetInstance().GetVelocity(), 2);
		fallVec_.y = max(fallVec_.y - gravity, -GRAVITY_MAX_);

		//���񂾂�キ
		fallVec_.z *= (FRAME_VEL_EXTEND_ + FRAME_VEL_EXTEND_REM * (1.0f - powf(GameVelocityManager::GetInstance().GetVelocity(), 2)));


		//�O��̈ʒu���獡�̈ʒu�̃x�N�g�������C�Ƃ��Ĕ���
		{
			RaycastHit info;

			//�G�ƒn�`�Ɣ�����Ƃ�
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
	fallVec_.y = 0;
}

void Weapon::Update()
{
	//��������e������
	if (!GetIsAlive() && owner_)
	{
		Character* chara = dynamic_cast<Character*>(owner_);
		owner_ = nullptr;
		chara->FallWeapon({ 0,0,0 });
	}

	Object::Update();
}
