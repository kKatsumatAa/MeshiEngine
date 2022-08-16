#pragma once
#include"Draw.h"

class Background
{
private:
	static const int backNum = 5;

	Draw back[backNum * 4];
	const Vec3 pos[4] =
	{
		{-100.0f,50.0f,0},//����
		{-100.0f,-50.0f,0},//����
		{100.0f,-50.0f,0},//��
		{100.0f,50.0f,0},//��
	};

	const float distance = 200.0f;

public:
	Background();
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

};

