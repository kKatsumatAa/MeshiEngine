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
	cursorPos_.x = Clamp(MouseInput::GetInstance().GetCurcorPos().x, cursorSize_.x * 2.0f, WindowsApp::WINDOW_WIDTH_ - cursorSize_.x * 2.0f);
	cursorPos_.y = Clamp(MouseInput::GetInstance().GetCurcorPos().y, 0, WindowsApp::WINDOW_HEIGHT_ - cursorSize_.y );


	//描画するときのカーソル位置
	drawPos_.x = (float)((cursorSize_.x / 2.0f) + (int32_t)cursorPos_.x / (int32_t)cursorSize_.x * (int32_t)cursorSize_.x);
	drawPos_.y = (float)((cursorSize_.y / 2.0f) + (int32_t)cursorPos_.y / (int32_t)cursorSize_.y * (int32_t)cursorSize_.y);
}

void CursorUI::Draw()
{
	//位置などをセット
	texSprite_.SetTrans({ drawPos_,0 });
	texSprite_.SetScale({ cursorSize_.x,cursorSize_.y,1.0f });
	texSprite_.SetColor({ 8.0f,0.2f,0.2f,alpha_ });
	//描画
	texSprite_.DrawBoxSprite(nullptr, { 0.5f,0.5f });
}
