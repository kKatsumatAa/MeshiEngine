#include "PlayerAttackState.h"
#include "CollisionManager.h"
#include "PlayerHand.h"
#include "Player.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

Vec3 PlayerAttackState::rotTmp_ = { 0,0,0 };


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

	//��ɂ���ĉ�]�p�x�t��
	if (playerHand_->GetIsRight())
	{
		rotTmp_.y = -PI;
	}
	else
	{
		rotTmp_.y = PI;
	}
	//�����_���Ɋp�x
	rotTmp_ = { GetRand(-PI / 8.0f,PI / 4.0f) ,rotTmp_.y / 2.0f,GetRand(-PI,PI) / 2.0f };
}

void PlayerAttackStateDoing::Update()
{
	float t = (float)timer_ / (float)TIMER_MAX_;

	//�o�ߎ��ԂŎ���ړ�
	Vec3 addTrans = LerpVec3({ 0,0,0 }, playerHand_->GetFrontVecTmp() * (playerHand_->GetInterLength() - playerHand_->GetScale().z / 2.0f), EaseOut(t));
	playerHand_->SetAddTrans(addTrans);

	//��]��
	Vec3 rot = LerpVec3({ 0,0,0 }, rotTmp_, EaseOut(t));
	playerHand_->SetRot(rot);

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
	Vec3 addTrans = LerpVec3(playerHand_->GetFrontVecTmp() * (playerHand_->GetInterLength() - playerHand_->GetScale().z / 2.0f), { 0,0,0 }, EaseOut(t));
	playerHand_->SetAddTrans(addTrans);

	//��]��
	Vec3 rot = LerpVec3(rotTmp_, { 0,0,0 }, EaseOut(t));
	playerHand_->SetRot(rot);

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
