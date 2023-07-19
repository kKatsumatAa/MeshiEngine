#include "ClearEffect.h"

ClearEffect& ClearEffect::GetInstance()
{
	static ClearEffect inst;
	return inst;
}

void ClearEffect::Initialize()
{
	timer_ = 0;
	t_ = 0;
	scale_ = 0;
	TextureManager::LoadGraph(L"clear.png", texHandle_);
	alpha_ = 0.0f;
}

void ClearEffect::Update()
{
	t_ = (float)timer_ / (float)TIMER_MAX_;

	scale_ = LerpVec3({ MAX_SCALE_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x_;

	alpha_ = t_;

	if (t_ > 1.0f)
	{
		timer_ = 0;
	}

	timer_++;
}

void ClearEffect::Draw()
{
	obj_.DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		scale_, { 2.5f,2.5f,2.5f,alpha_ }, texHandle_, { 0.5f,0.5f });
}
