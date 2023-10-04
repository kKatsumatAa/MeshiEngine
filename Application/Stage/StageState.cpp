#include "StageState.h"
#include "StageManager.h"
#include "MouseInput.h"
#include "PlayerUI.h"
#include "PostEffectManager.h"


//���ʂ̏���
void StageState::Initialize()
{
}

//���ʂ̏���
void StageState::Update()
{
	//�G�}�l�[�W���[
	EnemyManager::GetInstance().Update();

	//�e
	BulletManager::GetInstance().Update();

	//����
	CollisionManager::GetInstance()->CheckAllCollisions();

	//�v���C���[ui
	PlayerUI::GetInstance().Update();
}

//���ʂ̏���
void StageState::Draw()
{
	//�e
	BulletManager::GetInstance().Draw();

	//�G�}�l�[�W���[
	EnemyManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();
}

void StageState::DrawSprite()
{
}

//���ʂ̏���
void StageState::DrawImgui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
}


std::unique_ptr<StageState> StageState::GetState(const std::string& name)
{
	std::unique_ptr<StageState> state;

	if (name == "BEGINING")
	{
		state = std::make_unique<StageStateBegining>();
	}
	if (name == "BATTLE")
	{
		state = std::make_unique<StageStateBattle>();
	}
	if (name == "GAMEOVER")
	{
		state = std::make_unique<StageStateDead>();
	}
	if (name == "CLEAR")
	{
		state = std::make_unique<StageStateClear>();
	}

	return std::move(state);
}


//--------------------------------------------------------------------------
//�J�n��
void StageStateBegining::Initialize()
{
	StageState::Initialize();
}

void StageStateBegining::Update()
{
	//����
	StageState::Update();

	//�X�e�[�W�����X�Ƀf�B�]���u(���̃I�u�W�F�N�g���f�B�]���u�摜�ǂݍ���ł���O��)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;

	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);
	for (auto obj : objs)
	{
		obj->SetDissolveT(1.0f - t);
	}

	//���Ԓ�������
	if (timer_ >= EFFECT_TIMER_MAX_)
	{
		StageManager::GetInstance().ChangeState("BATTLE");
	}

	timer_++;
}

void StageStateBegining::Draw()
{
	StageState::Draw();
}

void StageStateBegining::DrawSprite()
{
	PlayerUI::GetInstance().DrawSprite();

	StageState::DrawSprite();
}


//--------------------------------------------------------------------------
//�퓬��
void StageStateBattle::Initialize()
{
	StageState::Initialize();
}

void StageStateBattle::Update()
{
	//����
	StageState::Update();

	//�`���[�g���A��
	Tutorial::GetInstance().Update();

	//�Q�[���I�[�o�[
	if (ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_ALLIES).size() <= 0)
	{
		StageManager::GetInstance().ChangeState("GAMEOVER");
	}
	//�Q�[���N���A
	else if (ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_ENEMYS).size() <= 0)
	{
		//�Q�[���X�s�[�h�̉��Z����
		GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(true);

		//�������������ɂ��Ă���
		GameVelocityManager::GetInstance().AddGameVelocity(CLEAR_ADD_GAME_VEL_);

		//���Ԓ�������
		if (timer_ >= CLEAR_COOL_TIMER_)
		{
			//�Q�[���X�s�[�h�̉��Z��������
			GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(false);

			StageManager::GetInstance().ChangeState("CLEAR");
		}

		timer_++;
	}
}

void StageStateBattle::Draw()
{
	//����
	StageState::Draw();
}

void StageStateBattle::DrawSprite()
{
	StageState::DrawSprite();

	PlayerUI::GetInstance().DrawSprite();

	//�`���[�g���A��
	Tutorial::GetInstance().Draw();
}


//--------------------------------------------------------------------------
//���S��
void StageStateDead::Initialize()
{
	StageState::Initialize();

	//�X�e�[�W�����X�Ƀf�B�]���u(���̃I�u�W�F�N�g���f�B�]���u�摜�ǂݍ���ł���O��)
	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);

	for (auto obj : objs)
	{
		obj->SetisDissolve(true);
	}
}

void StageStateDead::Update()
{
	//����
	StageState::Update();

	//�X�e�[�W�����X�Ƀf�B�]���u(���̃I�u�W�F�N�g���f�B�]���u�摜�ǂݍ���ł���O��)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;

	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);
	for (auto obj : objs)
	{
		obj->SetDissolveT(t);
	}

	//��莞�Ԃ�������t���O���Ă�
	if (timer_ >= EFFECT_TIMER_MAX_)
	{
		StageManager::GetInstance().SetIsGameOver(true);
	}

	timer_++;
}

void StageStateDead::Draw()
{
	//����
	StageState::Draw();
}

void StageStateDead::DrawSprite()
{
	StageState::DrawSprite();
}


//--------------------------------------------------------------------------
//�N���A��
void StageStateClear::Initialize()
{
	StageState::Initialize();

	//�N���A���o
	ClearEffect::GetInstance().BeginClearEffect();
}

void StageStateClear::Update()
{
	//����
	StageState::Update();

	//�N���A���o�p
	ClearEffect::GetInstance().Update();

	//�A�ł��Ă��N���A���o��΂��Ȃ��悤��
	if (MouseInput::GetInstance().GetClick(CLICK_LEFT) && timer_ >= CLEAR_COOL_TIME_MAX_)
	{
		StageManager::GetInstance().SetIsClear(true);
	}

	timer_ = min(timer_ + 1, CLEAR_COOL_TIME_MAX_);
}

void StageStateClear::Draw()
{
	//����
	StageState::Draw();
}

void StageStateClear::DrawSprite()
{
	StageState::DrawSprite();

	//�N���A���o
	ClearEffect::GetInstance().Draw();
}