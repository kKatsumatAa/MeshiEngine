#include "TitleUI.h"

TitleUI& TitleUI::GetInstance()
{
	static TitleUI sInst;
	return sInst;
}

void TitleUI::Initialize()
{
	titleTex_ =  TextureManager::LoadGraph("title.png");
	timer_ = 0;
	t_ = 0;
	titleScale_ = TITLE_SCALE_MAX_;

	//セット
	titleSprite_.SetTexHandle(titleTex_);
	titleSprite_.SetColor(SPRITE_COLOR_);
}

void TitleUI::Update()
{
	//タイトル画面の動き
	if (t_ < 1.0f)
	{
		t_ = (float)timer_ / (float)TITLE_TIME_MAX_;

		titleScale_ = Lerp(TITLE_SCALE_MAX_, 1.0f, EaseOut(t_));

		timer_++;
	}
}

void TitleUI::DrawSprite()
{
	//セット
	titleSprite_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	titleSprite_.SetScale({ titleScale_,titleScale_ ,1.0f });
	//描画
	titleSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
}
