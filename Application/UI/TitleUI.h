#pragma once
#include"Sprite.h"


class TitleUI final
{
private:
	float t_ = 0;
	int32_t timer_ = 0;
	const int32_t TITLE_TIME_MAX_ = 180;
	uint64_t titleTex_ = NULL;
	float titleScale_ = 0;
	const float TITLE_SCALE_MAX_ = 2.5f;
	Sprite titleSprite_;


private:
	TitleUI() { ; }
	~TitleUI() { ; }

public:
	TitleUI& operator=(const TitleUI& inst) = delete;
	TitleUI(const TitleUI& inst) = delete;

	static TitleUI& GetInstance();

public:
	void Initialize();
	void Update();
	void DrawSprite();
};