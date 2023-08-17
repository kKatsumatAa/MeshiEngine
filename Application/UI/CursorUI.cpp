#include "CursorUI.h"
#include "MouseInput.h"


CursorUI& CursorUI::GetInstance()
{
	static CursorUI sInst;
	return sInst;
}

void CursorUI::Initialize()
{
	cursorSize_ = { WindowsApp::GetInstance().WINDOW_WIDTH_ * cursorSizeExtend_.x_,
		WindowsApp::GetInstance().WINDOW_HEIGHT_ * cursorSizeExtend_.y_ };

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

	//カーソル位置
	drawPos_.x_ = (float)((cursorSize_.x_ / 2.0f) + (int32_t)MouseInput::GetInstance().GetCurcorPos().x_ / (int32_t)cursorSize_.x_ * (int32_t)cursorSize_.x_);
	drawPos_.y_ = (float)((cursorSize_.y_ / 2.0f) + (int32_t)MouseInput::GetInstance().GetCurcorPos().y_ / (int32_t)cursorSize_.y_ * (int32_t)cursorSize_.y_);
}

void CursorUI::Draw()
{
	texObj_.DrawBoxSprite(drawPos_,
		cursorSize_, { 1.0f,0.2f,0.2f,alpha_ }, NULL, { 0.5f,0.5f });
}
