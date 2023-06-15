#pragma once
#include"Object.h"

//
class DebugText
{
private:
	static const int32_t S_MAX_CHAR_COUNT_ = 256;
	static const int32_t S_FONT_WIDTH_ = 9;
	static const int32_t S_FONT_HEIGHT_ = 18;
	static const int32_t S_FONT_LINE_COUNT_ = 14;
	static const int32_t S_FONT_INDEX_MAX_X_ = 14;
	static const int32_t S_FONT_INDEX_MAX_Y_ = 7;

	Object sprites_[S_MAX_CHAR_COUNT_];
	Vec3 leftTop_[S_MAX_CHAR_COUNT_]; 
	float scale_[S_MAX_CHAR_COUNT_]; 
	XMFLOAT2 UVleftTop_[S_MAX_CHAR_COUNT_];
	XMFLOAT2 UVlength_[S_MAX_CHAR_COUNT_];

	//添え字用の変数
	int32_t spriteIndex_ = 0;

public:
	void Print(const std::string& text, float x, float y, int32_t variable = 114514, float scale = 1.0f);
	void Printf(const std::string& text, float x, float y, float variable = 114514.0f, float scale = 1.0f);
	
	void DrawAll(uint64_t texhandle);
};

