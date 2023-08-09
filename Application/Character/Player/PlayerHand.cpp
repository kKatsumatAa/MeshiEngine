#include "PlayerHand.h"
#include "PlayerAttackState.h"
#include "Player.h"


std::unique_ptr<PlayerHand> PlayerHand::Create(Player* player, const Vec3& transTmp)
{
	std::unique_ptr<PlayerHand> instance = std::make_unique<PlayerHand>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(player, transTmp))
	{
		assert(0);
	}

	return std::move(instance);
}

bool PlayerHand::Initialize(Player* player, const Vec3& transTmp)
{
	if (!Object::Initialize())
	{
		return false;
	}

	//�����蔻��͂Ȃ��̂ő������Ȃ�


	SetObjName("playerHand");

	//�����蔻��Ɏg��Ȃ�
	SetIsValid(false);

	player_ = player;
	isAttacking_ = false;
	//�e�q�֌W
	SetParent(player_);
	//���ŃX�P�[��
	SetScale({ 0.5f,0.5f,2.5f });
	//�U����ԕύX
	ChangeAttackState(std::make_unique<PlayerAttackStateNone>());

	//���ƂȂ���W
	transTmp_ = transTmp;
	SetTrans(transTmp_);

	return true;
}


void PlayerHand::Update()
{
	attackState_->Update();

	//������
	SetTrans(transTmp_ + addTrans_);

	Object::Update();
}

void PlayerHand::Draw()
{
	attackState_->Draw();

	DrawCube3D();
}

void PlayerHand::ChangeAttackState(std::unique_ptr<PlayerAttackState> state)
{
	attackState_.reset();
	//���L�����Ɠn��
	attackState_ = std::move(state);
	attackState_->SetPlayerHand(this);
	attackState_->Initialize();
}
