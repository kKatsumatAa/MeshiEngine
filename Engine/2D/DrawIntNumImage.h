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
		int32_t num;
		float scale;
		Vec2 numImageSize;
		bool isTrue;
	};

private:
	NumImage numImages_[5];
	//—]‚è
	int32_t numRemainder_;
	//‚»‚ÌŒ…
	int32_t numDigit_;

	uint64_t texhandle_;

	bool isStartDigit_ = false;

	int32_t numCount_ = 0;


public:
	void Initialize(uint64_t texhandle);

	void SetNum(int32_t num,const Vec2& pos, const Vec2& sizeUV, const Vec2& numImageSize, float scale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	void Draw();
};

