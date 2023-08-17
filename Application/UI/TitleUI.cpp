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
}

void TitleUI::Update()
{
	//ƒ^ƒCƒgƒ‹‰æ–Ê
	if (t_ < 1.0f)
	{
		t_ = (float)timer_ / (float)TITLE_TIME_MAX_;

		titleScale_ = LerpVec3({ TITLE_SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x_;

		timer_++;
	}
}

void TitleUI::DrawSprite()
{
	titleObj_.DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		{ titleScale_,titleScale_ }, { 10.0f,10.0f,10.0f,1.0f }, titleTex_, { 0.5f,0.5f });
}
