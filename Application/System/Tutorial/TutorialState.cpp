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

	//最初の演出
	if (!isAchievement_)
	{
		BeginingUpdate(BEGINING_TIMER_MAX_);
	}
	//条件達成後の演出
	else if (isAchievement_ && beginingTimer_ >= BEGINING_TIMER_MAX_)
	{
		//制限時間終わったら
		if (EndingUpdate(ENDING_TIMER_MAX_))
		{
			//次のステート
			tutorial_->ToNextState();
		}
	}
}

bool TutorialState::BeginingUpdate(const int32_t TIMER_MAX)
{
	t_ = min((float)beginingTimer_ / (float)TIMER_MAX, 1.0f);

	scale_ = LerpVec3({ MAX_SCALE_,0,0 }, { 1.0f,0,0 }, EaseIn(t_)).x;

	alpha_ = t_;

	//制限時間過ぎたら
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

	//制限時間過ぎたら
	if (t_ >= 1.0f)
	{
		return true;
	}

	endingTimer_++;

	return false;
}

void TutorialState::CommonSpriteDraw()
{
	texObj_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	texObj_.SetScale({ scale_,scale_ ,1.0f });

	texObj_.DrawBoxSprite(nullptr, texHandle_, { 2.5f,2.5f,2.5f,alpha_ }, { 0.5f,0.5f });
}

//----------------------------------------------------------------------------
//何もしない
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
//基本操作説明
void TutorialStateOperation::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial1.png");
	rateMax_ = 50;

	//player殴らせる
	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "player");
	if (objs.size())
	{
		for (auto obj : objs)
		{
			dynamic_cast<Player*>(obj)->SetIsClickLeft(true);
		}
	}
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
//ゲームスピード説明
void TutorialStateGameSpeed::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial2.png");
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
//マウス説明
void TutorialStateMouse::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial3.png");
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

