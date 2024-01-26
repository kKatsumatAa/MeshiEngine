#include "CursorUI.h"
#include "MouseInput.h"

using namespace Util;

const Vec2 CursorUI::cursorSize_ = { WindowsApp::WINDOW_WIDTH_ * 0.01f,
		WindowsApp::WINDOW_HEIGHT_ * 0.05f };


//----------------------------------------
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
	int32_t interval = timer_ % FLASHING_INTERVAL_ % (FLASHING_INTERVAL_ / flashingNum_);
	if (interval >= 0
		&& interval <= flashingIntervalWidth_)
	{
		alpha_ = 0;
	}
	else
	{
		alpha_ = NORMAL_ALPHA_;
	}

	//カーソルの位置を画面内に収める
	cursorPos_.x = Clamp(MouseInput::GetInstance().GetCurcorPos().x, cursorSize_.x * CLAMP_CURSOR_POS_SIZE_RATE_, 
		WindowsApp::WINDOW_WIDTH_ - cursorSize_.x * CLAMP_CURSOR_POS_SIZE_RATE_);
	cursorPos_.y = Clamp(MouseInput::GetInstance().GetCurcorPos().y, 0, WindowsApp::WINDOW_HEIGHT_ - cursorSize_.y);


	//描画するときのカーソル位置
	drawPos_.x = (float)((cursorSize_.x * DRAW_POS_ADD_SIZE_RATE_) + (int32_t)cursorPos_.x / (int32_t)cursorSize_.x * (int32_t)cursorSize_.x);
	drawPos_.y = (float)((cursorSize_.y * DRAW_POS_ADD_SIZE_RATE_) + (int32_t)cursorPos_.y / (int32_t)cursorSize_.y * (int32_t)cursorSize_.y);
}

void CursorUI::Draw()
{
	//位置などをセット
	texSprite_.SetTrans({ drawPos_,0 });
	texSprite_.SetScale({ cursorSize_.x,cursorSize_.y,SPRITE_NORMAL_SCALE_ });
	texSprite_.SetColor({ SPRITE_COLOR_.x,SPRITE_COLOR_.y,SPRITE_COLOR_.z,alpha_ });
	//描画
	texSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
}
