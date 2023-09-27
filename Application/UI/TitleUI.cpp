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
}

void TitleUI::Update()
{
	//ƒ^ƒCƒgƒ‹‰æ–Ê
	if (t_ < 1.0f)
	{
		t_ = (float)timer_ / (float)TITLE_TIME_MAX_;

		titleScale_ = LerpVec3({ TITLE_SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x;

		timer_++;
	}
}

void TitleUI::DrawSprite()
{
	titleObj_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	titleObj_.SetScale({ titleScale_,titleScale_ ,1.0f });

	titleObj_.DrawBoxSprite(nullptr, titleTex_, { 10.0f,10.0f,10.0f,1.0f }, { 0.5f,0.5f });
}
