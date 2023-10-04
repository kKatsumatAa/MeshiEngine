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

	//�Z�b�g
	titleSprite_.SetTexHandle(titleTex_);
	titleSprite_.SetColor({ 1.0f,1.0f,1.0f,1.0f });
}

void TitleUI::Update()
{
	//�^�C�g����ʂ̓���
	if (t_ < 1.0f)
	{
		t_ = (float)timer_ / (float)TITLE_TIME_MAX_;

		titleScale_ = LerpVec3({ TITLE_SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x;

		timer_++;
	}
}

void TitleUI::DrawSprite()
{
	//�Z�b�g
	titleSprite_.SetTrans({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f ,0 });
	titleSprite_.SetScale({ titleScale_,titleScale_ ,1.0f });
	//�`��
	titleSprite_.DrawBoxSprite(nullptr, { 0.5f,0.5f });
}
