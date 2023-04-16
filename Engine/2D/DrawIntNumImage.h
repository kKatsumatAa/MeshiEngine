#pragma once
#include"Object.h"

class DrawIntNumImage
{
private:
	struct NumImage
	{
		Object obj;
		XMFLOAT4 color;
		Vec2 sizeUV;
		Vec2 pos;
		int num;
		float scale;
		Vec2 numImageSize;
		bool isTrue;
	};

private:
	NumImage numImages[5];
	//—]‚è
	int numRemainder;
	//‚»‚ÌŒ…
	int numDigit;

	UINT64 texhandle;

	bool isStartDigit = false;

	int numCount = 0;


public:
	void Initialize(UINT64 texhandle);

	void SetNum(int num, Vec2 pos, Vec2 sizeUV, Vec2 numImageSize, float scale, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });

	void Draw();
};

