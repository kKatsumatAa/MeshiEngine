#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "ObjectManager.h"



bool PlayerState::CheckEyeRayHit()
{
	//�U���ł��邩�ǂ���
//���C�Ƀv���C���[�̐��ʃx�N�g�������
	Ray ray;
	ray.dir = { player_->GetFrontVec().x_,player_->GetFrontVec().y_,player_->GetFrontVec().z_ };
	ray.start = { player_->GetTrans().x_,player_->GetTrans().y_,player_->GetTrans().z_ };

	//���ʃx�N�g���ɉ������邩
	uint16_t attribute = COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS;
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, attribute, &info_, player_->GetAttackLength());

	//�V���G�b�g�I�t
	auto objs = ObjectManager::GetInstance().GetObjs();
	for (auto obj : objs)
	{
		obj->effectFlags_.isSilhouette = false;
	}

	//�V���G�b�g
	if (isRayHit)
	{
		info_.object->SetIsSilhouette(true);
		info_.object->EffectUpdate();
	}
	return isRayHit;
}


//�f����-----------------------------------------------------------------------
void PlayerStateBareHands::Initialize()
{
}

void PlayerStateBareHands::Update()
{
	bool isRayHit = CheckEyeRayHit();

	//��������Ə��ɂ�������
	if (isRayHit)
	{
		player_->SetIsTarget(true);

		//����������ĂȂ��Ď����傪���Ȃ�����ɏƏ��������Ă���
		if ((info_.object->GetObjName() == "gun" || info_.object->GetObjName() == "sword")
			&& player_->GetWeapon() == nullptr && info_.object->GetParent() == nullptr)
		{
			//ui�ύX
			PlayerUI::GetInstance().ChangeState("PICKUP");

			if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
			{
				Weapon* weapon = dynamic_cast<Weapon*>(info_.object);
				//����E��
				Vec3 localPos = { player_->GetScale().x_ ,-player_->GetScale().y_ / 2.0f ,player_->GetScale().z_ * 2.0f };
				player_->PickUpWeapon(weapon, &localPos);

				//�v���C���[�͋t�����Ȃ̂ŉ���
				weapon->SetRotY(PI);

				//ui�ύX
				PlayerUI::GetInstance().ChangeState("GUN");

				//�X�e�[�g�ύX
				player_->ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
			}
		}
		//�G���Ə��ɂ������牣��
		else if (info_.object->GetObjName() == "enemy")
		{
			//ui�ύX
			PlayerUI::GetInstance().ChangeState("PUNCH");

			player_->GetHandManager()->Attack(info_);
		}
	}
	//�Ȃ����
	else
	{
		//ui�ύX
		PlayerUI::GetInstance().ChangeState("NORMAL");

		player_->SetIsTarget(false);
	}
}


//���펝���Ă���-----------------------------------------------------------------------
void PlayerStateHaveWeapon::Initialize()
{
}

void PlayerStateHaveWeapon::Update()
{
	//�v���C���[������������Ă�����
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//�N�[���^�C����UI��]
		PlayerUI::GetInstance().SetAngle(-360.0f * player_->GetWeapon()->GetAttackCoolTimeRatio());

		//�N���b�N�ōU��
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			//�e�������Ȃ���Γ�����
			if (player_->GetWeapon()->GetObjName() == "gun")
			{
				Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
				if (gun->GetBulletNum() <= 0)
				{
					player_->ThrowWeapon();
					return;
				}
			}

			//�U��
			player_->GetWeapon()->Attack(player_->GetFrontVec(), 1, player_);
			//�Q�[���X�s�[�h���Z
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//�E�N���b�N�ŕ��퓊����
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			player_->ThrowWeapon();
		}
	}
}


