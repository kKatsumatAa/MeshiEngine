#pragma once
#include"Draw.h"

static const int maxCharCount = 256;
static const int fontWidth = 9;
static const int fontHeight = 18;
static const int fontLineCount = 14;
static const int fontIndexMaxX = 14;
static const int fontIndexMaxY = 7;


class DebugText
{
private:
	Draw sprites[maxCharCount];

	//“Y‚¦š—p‚Ì•Ï”
	int spriteIndex = 0;

public:
	/*DebugText();*/
	void Print(const std::string& text, float x, float y, UINT64 texhandle, int variable = NULL, float scale = 1.0f);
	void Print(const std::string& text, float x, float y, UINT64 texhandle, float variable = NULL, float scale = 1.0f);

};

