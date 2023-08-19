#include "CursorUI.h"
#include "MouseInput.h"


const  Vec2 CursorUI::cursorSize_ = { WindowsApp::WINDOW_WIDTH_ * 0.01f,
		WindowsApp::WINDOW_HEIGHT_ * 0.05f };

CursorUI& CursorUI::GetInstance()
{
	static CursorUI sInst;
	return sInst;
}

void CursorUI::Initialize()
{
}

void CursorUI::Update()
{
	timer_++;

	//点滅させる
	int32_t interval = timer_ % 60 % (60 / flashingNum_);
	if (interval >= 0
		&& interval <= flashingIntervalWidth_)
	{
		alpha_ = 0;
	}
	else
	{
		alpha_ = 1.0f;
	}

	//カーソルの位置を画面内に収める
	cursorPos_.x_ = Clamp(MouseInput::GetInstance().GetCurcorPos().x_, 0, WindowsApp::WINDOW_WIDTH_ - cursorSize_.x_);
	cursorPos_.y_ = Clamp(MouseInput::GetInstance().GetCurcorPos().y_, 0, WindowsApp::WINDOW_HEIGHT_ - cursorSize_.y_);


	//描画するときのカーソル位置
	drawPos_.x_ = (float)((cursorSize_.x_ / 2.0f) + (int32_t)cursorPos_.x_ / (int32_t)cursorSize_.x_ * (int32_t)cursorSize_.x_);
	drawPos_.y_ = (float)((cursorSize_.y_ / 2.0f) + (int32_t)cursorPos_.y_ / (int32_t)cursorSize_.y_ * (int32_t)cursorSize_.y_);
}

void CursorUI::Draw()
{
	texObj_.DrawBoxSprite(drawPos_,
		cursorSize_, { 1.0f,0.2f,0.2f,alpha_ }, NULL, { 0.5f,0.5f });
}
