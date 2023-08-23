#include "EnemyState.h"
#include "Enemy.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"



bool EnemyState::CheckEyeRayHit()
{
	bool isRayHit = enemy_->CheckRayOfEyeHit(enemy_->GetFrontVec(),
		Enemy::S_LENGTH_MAX_, COLLISION_ATTR_ALLIES, &info_);

	return isRayHit;
}

Vec3 EnemyState::GetRayHitGunOrPlayerPos()
{
	auto guns = ObjectManager::GetInstance().GetObjs("gun");

	float minLength = Enemy::S_LENGTH_MAX_;
	bool gunSee = false;
	Vec3 gunPos;

	//�S�Ă̏e�Ɍ������ă��C���΂��A��Q�����Ȃ����
	for (auto gun : guns)
	{
		if (enemy_->CheckRayOfEyeHit((gun->GetWorldTrans() - enemy_->GetWorldTrans()).GetNormalized(),
			Enemy::S_LENGTH_MAX_, BARE_HANDS_ATTR_TMP_, &info_))
		{
			//�e�ŏ��L�҂����Ȃ����
			if (info_.object->GetObjName() == "gun" && info_.object->GetParent() == nullptr)
			{
				//����
				float distance = (info_.object->GetTrans() - enemy_->GetWorldTrans()).GetLength();

				//��ԋ߂���Εۑ�
				if (minLength > distance)
				{
					minLength = distance;
					gunPos = info_.object->GetTrans();
				}
				gunSee = true;
			}
		}
	}

	//�e�����E�ɂ����
	if (gunSee)
	{
		return gunPos;
	}

	//�Ȃ���΃v���C���[��
	return CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
}


//�f����-----------------------------------------------------------------------
void EnemyStateBareHands::Initialize()
{
	enemy_->SetIsPlayAnimation(true);
	enemy_->SetIsLoopAnimation(true);
}

void EnemyStateBareHands::Update()
{
	enemy_->Move(GetRayHitGunOrPlayerPos());

	//���펝������X�e�[�g�ύX
	if (enemy_->GetWeapon())
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateHaveWeapon>());
	}
}


//���펝���Ă���-----------------------------------------------------------------------
void EnemyStateHaveWeapon::Initialize()
{
}

void EnemyStateHaveWeapon::Update()
{
	//�v���C���[�̕����Ƀ��C���΂���
	if (enemy_->CheckRayOfEyeHit(
		(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye() - enemy_->GetWorldTrans()).GetNormalized(),
		Enemy::S_LENGTH_MAX_, HAVE_WEAPON_ATTR_TMP_, &info_)
		)
	{
		//�v���C���[���������瓮�����U��
		if (info_.object->GetObjName() == "player")
		{
			enemy_->SetIsPlayAnimation(false);
			enemy_->SetIsLoopAnimation(false);
			enemy_->Attack(info_.object->GetWorldTrans());
			enemy_->DirectionUpdate(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());
		}
		//���j�����Ȃ���΃v���C���[�̕����Ɉړ�
		else
		{
			enemy_->SetIsPlayAnimation(true);
			enemy_->SetIsLoopAnimation(true);
			enemy_->Move(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());
		}
	}

	//���편������X�e�[�g�ύX
	if (enemy_->GetWeapon() == nullptr)
	{
		enemy_->ChangeEnemyState(std::make_unique<EnemyStateBareHands>());
	}
}


