#pragma once
#include"Draw.h"

class Background
{
private:
	static const int backNum = 8;

	Draw back[backNum * 4];
	const Vec3 pos[4] =
	{
		{-128.0f,72.0f,0},//ç∂è„
		{-128.0f,-72.0f,0},//ç∂â∫
		{128.0f,-72.0f,0},//â∫
		{128.0f,72.0f,0},//è„
	};

	const float distance = 300.0f;

	float count = 0;

public:
	int phase = 0;

	void Initialize();

	Background();
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle, const bool& boss = false);

};

