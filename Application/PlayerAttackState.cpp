#include "PlayerAttackState.h"
#include "CollisionManager.h"
#include "PlayerHand.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"


//�������Ȃ�---------------------------------------------------------------
void PlayerAttackStateNone::Initialize()
{
}

void PlayerAttackStateNone::Update()
{

}

void PlayerAttackStateNone::Draw()
{
}


//�U��1---------------------------------------------------------------
void PlayerAttackStateDoing::Initialize()
{
	playerHand_->SetIsAttacking(true);
}

void PlayerAttackStateDoing::Update()
{
	float t = (float)timer_ / (float)TIMER_MAX_;

	//�o�ߎ��ԂŎ���ړ�
	Vec3 addTrans = LerpVec3({ 0,0,0 }, Vec3(0, 0, 1.0f) * (playerHand_->GetInterLength() - playerHand_->GetScale().z_), EaseOut(t));

	playerHand_->SetAddTrans(addTrans);

	//���Ԍo�߂��Q�[���X�s�[�h��������
	timer_ += 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//�X�e�[�g�ύX
	if (t >= 1.0f)
	{
		//�G�̔�e����
		enemyDamageFunc_();

		playerHand_->ChangeAttackState(std::make_unique<PlayerAttackStateDoing2>());
	}

}

void PlayerAttackStateDoing::Draw()
{
}


//�U��2�i�߂��j---------------------------------------------------------------
void PlayerAttackStateDoing2::Initialize()
{
}

void PlayerAttackStateDoing2::Update()
{
	float t = (float)timer_ / (float)TIMER_MAX_;

	//�o�ߎ��ԂŎ��߂�
	Vec3 addTrans = LerpVec3(Vec3(0, 0, 1.0f) * (playerHand_->GetInterLength() - playerHand_->GetScale().z_), { 0,0,0 }, EaseOut(t));

	playerHand_->SetAddTrans(addTrans);

	//���Ԍo�߂��Q�[���X�s�[�h��������
	timer_ += 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//�X�e�[�g�ύX
	if (t >= 1.0f)
	{
		playerHand_->SetIsAttacking(false);

		playerHand_->ChangeAttackState(std::make_unique<PlayerAttackStateNone>());
	}
}

void PlayerAttackStateDoing2::Draw()
{
}
