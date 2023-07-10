#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"



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

	if (isRayHit)
	{
		info_.object->SetIsSilhouette(true);
		info_.object->Update();
	}
	return isRayHit;
}

void PlayerState::ThrowGun()
{
	player_->FallWeapon(player_->GetFrontVec() * 5.0f);

	//�Q�[���X�s�[�h���Z
	GameVelocityManager::GetInstance().AddGameVelocity(0.9f);

	//�X�e�[�g�ύX(�f��)
	player_->ChangePlayerState(std::make_unique<PlayerStateBareHands>());
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

		//�e�������ĂȂ��Ď����傪���Ȃ��e�ɏƏ��������Ă���
		if (info_.object->GetObjName() == "gun" && player_->GetWeapon() == nullptr && info_.object->GetParent() == nullptr
			&& MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			Gun* gun = dynamic_cast<Gun*>(info_.object);
			//�e�E��
			Vec3 localPos = { player_->GetScale().x_ ,-player_->GetScale().y_ / 2.0f ,player_->GetScale().z_ * 2.0f };
			player_->PickUpWeapon(gun, &localPos);

			//�v���C���[�͋t�����Ȃ̂ŉ���
			gun->SetRotY(PI);

			//�X�e�[�g�ύX(�e)
			player_->ChangePlayerState(std::make_unique<PlayerStateHaveGun>());
		}
		//�G���Ə��ɂ������牣��
		else if (info_.object->GetObjName() == "enemy")
		{
			player_->GetHandManager()->Attack(info_);
		}
	}
	//�Ȃ����
	else
	{
		player_->SetIsTarget(false);
	}
}


//�e�����Ă���-----------------------------------------------------------------------
void PlayerStateHaveGun::Initialize()
{
}

void PlayerStateHaveGun::Update()
{
	//�v���C���[���e�������Ă�����
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//�N���b�N�Ō���
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			//�e�������Ȃ���Γ�����
			Gun* gun = dynamic_cast<Gun*>(player_->GetWeapon());
			if (gun->GetBulletNum() <= 0)
			{
				ThrowGun();
				return;
			}

			player_->GetWeapon()->Attack(player_->GetFrontVec(), 1, player_);
			//�Q�[���X�s�[�h���Z
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//�E�N���b�N�ŏe������
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			ThrowGun();
		}
	}
}


