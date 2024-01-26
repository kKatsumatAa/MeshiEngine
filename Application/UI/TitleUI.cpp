#include "TitleUI.h"

using namespace Util;


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
	if (t_ < TIME_RATIO_MAX_)
	{
		t_ = (float)timer_ / (float)TITLE_TIME_MAX_;

		titleScale_ = Lerp(TITLE_SCALE_MAX_, NORMAL_SCALE_, EaseOut(t_));

		timer_++;
	}
}

void TitleUI::DrawSprite()
{
	//セット
	titleSprite_.SetTrans(SPRITE_POS_);
	titleSprite_.SetScale({ titleScale_,titleScale_ ,NORMAL_SCALE_ });
	//描画
	titleSprite_.DrawBoxSprite(nullptr, SPRITE_ANCOR_UV_);
}
