#include "TutorialState.h"
#include "Tutorial.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"
#include "Player.h"

using namespace Util;


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
	else if (name == "PART")
	{
		state = std::make_unique<TutorialStatePart>();
	}
	else if (name == "PART2")
	{
		state = std::make_unique<TutorialStatePart2>();
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
	t_ = min((float)beginingTimer_ / (float)TIMER_MAX, TIMER_RATE_MAX_);

	scale_ = Lerp(MAX_SCALE_, NORMAL_SCALE_, EaseIn(t_));

	alpha_ = t_;

	//制限時間過ぎたら
	if (t_ >= TIMER_RATE_MAX_)
	{
		return true;
	}

	beginingTimer_++;

	return false;
}

bool TutorialState::EndingUpdate(const int32_t TIMER_MAX)
{
	t_ = min((float)endingTimer_ / (float)TIMER_MAX, TIMER_RATE_MAX_);

	scale_ = Lerp(NORMAL_SCALE_, MAX_SCALE_, EaseIn(t_));

	alpha_ = TIMER_RATE_MAX_ - t_;

	//制限時間過ぎたら
	if (t_ >= TIMER_RATE_MAX_)
	{
		return true;
	}

	endingTimer_++;

	return false;
}

void TutorialState::CommonSpriteDraw()
{
	texSprite_.SetTrans(SPRITE_POS_);
	texSprite_.SetScale({ scale_,scale_ ,NORMAL_SCALE_ });
	texSprite_.SetColor({ SPRITE_COLOR_.x,SPRITE_COLOR_.y,SPRITE_COLOR_.z,alpha_ });

	texSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
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
	//テクスチャハンドルセット
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial1.dds");
	texSprite_.SetTexHandle(texHandle_);

	//ステートごとに最大値を変えている
	rateMax_ = RATE_MAX_TMP_;
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
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial2.dds");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = RATE_MAX_TMP_;
}

void TutorialStateGameSpeed::Update()
{
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
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial3.dds");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = RATE_MAX_TMP_;
}

void TutorialStateMouse::Update()
{
	CommonUpdate();
}

void TutorialStateMouse::Draw()
{
	CommonSpriteDraw();
}

//--------------------------------------------------------------------------
//部位説明
void TutorialStatePart::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial4.dds");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = RATE_MAX_TMP_;
}

void TutorialStatePart::Update()
{
	rate_ += GameVelocityManager::GetInstance().GetVelocity();

	CommonUpdate();
}

void TutorialStatePart::Draw()
{
	CommonSpriteDraw();
}

//--------------------------------------------------------------------------
//部位説明2
void TutorialStatePart2::Initialize()
{
	texHandle_ = TextureManager::GetInstance().LoadGraph("tutorial5.dds");
	texSprite_.SetTexHandle(texHandle_);

	rateMax_ = RATE_MAX_TMP_;
}

void TutorialStatePart2::Update()
{
	rate_ += GameVelocityManager::GetInstance().GetVelocity();

	CommonUpdate();
}

void TutorialStatePart2::Draw()
{
	CommonSpriteDraw();
}
