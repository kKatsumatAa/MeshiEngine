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
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, "player", &info_, player_->GetAttackLength());

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

		//�e�������ĂȂ��Ď����傪���Ȃ��e�ɏƏ��������Ă���
		if (info_.object->GetObjName() == "gun" && player_->GetGun() == nullptr && info_.object->GetParent() == nullptr
			&& MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			Gun* gun = dynamic_cast<Gun*>(info_.object);
			player_->SetGun(gun);
			Vec3 localP = { player_->GetScale().x_ ,-player_->GetScale().y_ * 2.0f ,player_->GetScale().z_ * 2.0f };
			gun->SetLocalPos(localP);
			gun->ChangeOwner(player_->GetWorldMat());

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
	if (player_->GetGun() != nullptr)
	{
		player_->SetIsTarget(false);

		//�N���b�N�Ō���
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
		{
			player_->GetGun()->Shot(player_->GetFrontVec());
			//�Q�[���X�s�[�h���Z
			GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
		}
		//�E�N���b�N�ŏe������
		else if (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT))
		{
			player_->GetGun()->ChangeOwner(nullptr);
			player_->GetGun()->SetFallVec(player_->GetFrontVec() * 10.0f);
			player_->SetGun(nullptr);

			//�X�e�[�g�ύX(�f��)
			player_->ChangePlayerState(std::make_unique<PlayerStateBareHands>());
		}
	}
}


