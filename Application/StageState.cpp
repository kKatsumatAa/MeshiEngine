#include "StageState.h"
#include "StageManager.h"
#include "MouseInput.h"


//���ʂ̏���
void StageState::Initialize()
{
}

//���ʂ̏���
void StageState::Update()
{
	//���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g��
	LevelManager::GetInstance().Update();

	//�e
	BulletManager::GetInstance().Update();

	//����
	CollisionManager::GetInstance()->CheckAllCollisions();

	//�Q�[���X�s�[�h
	GameVelocityManager::GetInstance().Update();
}

//���ʂ̏���
void StageState::Draw()
{
	//�e
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	ParticleManager::GetInstance()->Draw();
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
	LevelManager::GetInstance().SetIsDissolveT(1.0f - t, COLLISION_ATTR_LANDSHAPE);

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
	if (LevelManager::GetInstance().GetGameOver())
	{
		StageManager::GetInstance().ChangeState("GAMEOVER");
	}
	//�Q�[���N���A
	else if (LevelManager::GetInstance().GetGameClear())
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

	//�`���[�g���A��
	Tutorial::GetInstance().Draw();
}

void StageStateBattle::DrawSprite()
{
}


//--------------------------------------------------------------------------
//���S��
void StageStateDead::Initialize()
{
	StageState::Initialize();

	//�X�e�[�W�����X�Ƀf�B�]���u(���̃I�u�W�F�N�g���f�B�]���u�摜�ǂݍ���ł���O��)
	LevelManager::GetInstance().SetObjectIsDissolve(true, COLLISION_ATTR_LANDSHAPE);
}

void StageStateDead::Update()
{
	//����
	StageState::Update();

	//�X�e�[�W�����X�Ƀf�B�]���u(���̃I�u�W�F�N�g���f�B�]���u�摜�ǂݍ���ł���O��)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;
	LevelManager::GetInstance().SetIsDissolveT(t, COLLISION_ATTR_LANDSHAPE);

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

	//�N���A���o
	ClearEffect::GetInstance().Draw();
}

void StageStateClear::DrawSprite()
{
}