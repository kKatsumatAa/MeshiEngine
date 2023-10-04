#include "TutorialState.h"
#include "Tutorial.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"
#include "Player.h"



std::unique_ptr<TutorialState> TutorialState::GetState(const std::string& name)
{
	std::unique_ptr<TutorialState> state = nullptr;

	if (name == "OPERATION")
	{
		state = std::make_unique<TutorialStateOperation>();
	}
	else if (name == "GAMESPEED")
	{
		state = std::make_unique<TutorialStateGameSpeed>();
	}
	else if (name == "MOUSE")
	{
		state = std::make_unique<TutorialStateMouse>();
	}
	else
	{
		state = std::make_unique<TutorialStateNone>();
	}

	return std::move(state);
}

void TutorialState::CommonUpdate()
{
	if (rate_ >= rateMax_)
	{
		isAchievement_ = true;
	}

	//�ŏ��̉��o
	if (!isAchievement_)
	{
		BeginingUpdate(BEGINING_TIMER_MAX_);
	}
	//�����B����̉��o
	else if (isAchievement_ && beginingTimer_ >= BEGINING_TIMER_MAX_)
	{
		//�������ԏI�������
		if (EndingUpdate(ENDING_TIMER_MAX_))
		{
			//���̃X�e�[�g
			tutorial_->ToNextState();
		}
	}
}

bool TutorialState::BeginingUpdate(const int32_t TIMER_MAX)
{
	t_ = min((float)beginingTimer_ / (float)TIMER_MAX, 1.0f);

	scale_ = LerpVec3({ MAX_SCALE_,0,0 }, { 1.0f,0,0 }, EaseIn(t_)).x;

	alpha_ = t_;

	//�������ԉ߂�����
	if (t_ >= 1.0f)
	{
		return true;
	}

	beginingTimer_++;

	return false;
}

bool TutorialState::EndingUpdate(const int32_t TIMER_MAX)
{
	t_ = min((float)endingTimer_ / (float)TIMER_MAX, 1.0f);

	scale_ = LerpVec3({ 1.0f,0,0 }, { MAX_SCALE_,0,0 }, EaseIn(t_)).x;

	alpha_ = 1.0f - t_;

	//�������ԉ߂�����
	if (t_ >= 1.0f)
	{
		return true;
	}

	endingTimer_++;

	return false;
}

void TutorialState::CommonSpriteDraw()
{
	texSprite_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	texSprite_.SetScale({ scale_,scale_ ,1.0f });
	texSprite_.SetColor({ 1.0f,1.0f,1.0f,alpha_ });

	texSprite_.DrawBoxSprite(nullptr, { 0.5f,0.5f });
}

//----------------------------------------------------------------------------
//�������Ȃ�
void TutorialStateNone::Initialize()
{
}

void TutorialStateNone::Update()
{
}

void TutorialStateNone::Draw()
{
}


//----------------------------------------------------------------------------
//��{�������
void TutorialStateOperation::Initialize()
{
	//�e�N�X�`���n���h���Z�b�g
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial1.png");
	texSprite_.SetTexHandle(texHandle_);
	
	//�X�e�[�g���Ƃɍő�l��ς��Ă���
	rateMax_ = 50;
}

void TutorialStateOperation::Update()
{
	KeyboardInput& key = KeyboardInput::GetInstance();

	if (key.KeyPush(DIK_A) || key.KeyPush(DIK_S) || key.KeyPush(DIK_W) || key.KeyPush(DIK_D))
	{
		rate_++;
	}
	rate_ += GameVelocityManager::GetInstance().GetVelocity();

	CommonUpdate();
}

void TutorialStateOperation::Draw()
{
	CommonSpriteDraw();
}


//----------------------------------------------------------------------------
//�Q�[���X�s�[�h����
void TutorialStateGameSpeed::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial2.png");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = 30;
}

void TutorialStateGameSpeed::Update()
{
	KeyboardInput& key = KeyboardInput::GetInstance();

	rate_ += GameVelocityManager::GetInstance().GetVelocity();

	CommonUpdate();
}

void TutorialStateGameSpeed::Draw()
{
	CommonSpriteDraw();
}

//----------------------------------------------------------------------------
//�}�E�X����
void TutorialStateMouse::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial3.png");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = 100;
}

void TutorialStateMouse::Update()
{
	CommonUpdate();
}

void TutorialStateMouse::Draw()
{
	CommonSpriteDraw();
}

