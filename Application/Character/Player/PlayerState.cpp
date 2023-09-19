#include "PlayerState.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "ObjectManager.h"
#include "CameraManager.h"



bool PlayerState::CheckEyeRayHit()
{
	bool isRayHit = player_->CheckRayOfEyeHit(player_->GetFrontVec(),
		player_->GetAttackLength(), COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS, &info_);

	//�V���G�b�g�I�t
	auto objs = ObjectManager::GetInstance().GetObjs();
	for (auto obj : objs)
	{
		obj->SetIsSilhouette(false);
	}

	//�V���G�b�g
	if (isRayHit)
	{
		info_.object->SetIsSilhouette(true);
		info_.object->EffectUpdate();
	}

	return isRayHit;
}

void PlayerState::Update()
{
	//�J�����̌����ύX
	player_->DirectionUpdate();

	//�ړ�
	player_->Move();
}


//�f����-----------------------------------------------------------------------
void PlayerStateBareHands::Initialize()
{
}

void PlayerStateBareHands::Update()
{
	PlayerState::Update();

	bool isRayHit = PlayerState::CheckEyeRayHit();

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

			if (player_->GetIsClickLeft())
			{
				Weapon* weapon = dynamic_cast<Weapon*>(info_.object);
				//����E��
				Vec3 localPos = { -player_->GetScale().x ,-player_->GetScale().y / 2.0f ,-player_->GetScale().z * 2.0f };
				player_->PickUpWeapon(weapon, &localPos);

				//ui�ύX
				PlayerUI::GetInstance().ChangeState("GUN");

				//�X�e�[�g�ύX
				player_->ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
			}
		}
		//�G���Ə��ɂ������牣��
		else if (player_->GetIsClickLeft() &&
			info_.object->GetObjName().find("enemy") != std::string::npos)
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
	//����ui�ύX
	PlayerUI::GetInstance().ChangeState("GUN");
}

void PlayerStateHaveWeapon::Update()
{
	PlayerState::Update();

	//�v���C���[������������Ă�����
	if (player_->GetWeapon() != nullptr)
	{
		player_->SetIsTarget(false);

		//�N�[���^�C����UI��]
		PlayerUI::GetInstance().SetAngle(-PI * 2.0f * player_->GetWeapon()->GetAttackCoolTimeRatio());

		//�N���b�N�ōU��
		if (player_->GetIsClickLeft())
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


//-----------------------------------------------------------------------------------------
//���S���o
void PlayerStateDeadEffect::Initialize()
{
	targetPos_ = player_->GetBulletOwnerEnemyPos();
	dir_ = (targetPos_ - player_->GetWorldTrans()).GetNormalized();

	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");
	camera->SetTarget(targetPos_);
}

void PlayerStateDeadEffect::Update()
{
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	float t = (float)timer_ / (float)TIMER_MAX_;

	camera->SetEye(LerpVec3(targetPos_ - (dir_ * LENGTH_MIN_), player_->GetWorldTrans(), EaseInOut(t)));

	//���o�I������琶���t���O�I�t
	if (timer_ >= TIMER_MAX_)
	{
		player_->SetIsAlive(false);
	}

	timer_++;
}
