#pragma once
#include"Object.h"

static const int32_t maxCharCount = 256;
static const int32_t fontWidth = 9;
static const int32_t fontHeight = 18;
static const int32_t fontLineCount = 14;
static const int32_t fontIndexMaxX = 14;
static const int32_t fontIndexMaxY = 7;

//
class DebugText
{
private:
	Object sprites[maxCharCount];
	Vec3 leftTop[maxCharCount]; float scale[maxCharCount]; XMFLOAT2 UVleftTop[maxCharCount]; XMFLOAT2 UVlength[maxCharCount];

	//ìYÇ¶éöópÇÃïœêî
	int32_t spriteIndex = 0;

public:
	void Print(const std::string& text, float x, float y, int32_t variable = 114514, float scale = 1.0f);
	void Printf(const std::string& text, float x, float y, float variable = 114514.0f, float scale = 1.0f);
	
	void DrawAll(uint64_t texhandle);
};

