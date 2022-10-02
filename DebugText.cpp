#include "DebugText.h"
using namespace std;


void DebugText::Print(const std::string& text, float x, float y, UINT64 texhandle, int variable, float scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数を超えたら
		if (spriteIndex >= maxCharCount) break;

		//1文字を取り出す
		const unsigned char& character = text[i];

		//ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f) fontIndex = 0;

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex].DrawClippingBoxSprite(
			{ x + i * fontWidth * scale,y,0 },
			scale,
			{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY) ) - 0.003f },
			{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
			{ 1.0f,1.0f,1.0f,1.0f }, 
			texhandle);
	
		//添え字用変数をインクリメント
		spriteIndex++;
	}

//変数の中身表示用------------------------
	if (variable != NULL)
	{
		int number = variable;
		int printNumber[6] = { };

		printNumber[0] = number / 100000;
		number = number % 100000;

		printNumber[1] = number / 10000;
		number = number % 10000;

		printNumber[2] = number / 1000;
		number = number % 1000;

		printNumber[3] = number / 100;
		number = number % 100;

		printNumber[4] = number / 10;
		number = number % 10;

		printNumber[5] = number;
			

		for (int i = 0; i < _countof(printNumber); i++)
		{
			//ASCIIコードの1段分飛ばした番号を計算
			int fontIndex = printNumber[i] + 16;
			//if (character >= 0x7f) fontIndex = 0;

			int fontIndexY = fontIndex / fontLineCount;
			int fontIndexX = fontIndex % fontLineCount;

			//座標計算
			sprites[spriteIndex].DrawClippingBoxSprite(
				{ x + (i + text.size()) * fontWidth * scale,y,0 },
				scale,
				{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
				{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
				{ 1.0f,1.0f,1.0f,1.0f },
				texhandle);

			//添え字用変数をインクリメント
			spriteIndex++;
		}
	}

	spriteIndex = 0;
}

void DebugText::Print(const std::string& text, float x, float y, UINT64 texhandle, float variable, float scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数を超えたら
		if (spriteIndex >= maxCharCount) break;

		//1文字を取り出す
		const unsigned char& character = text[i];

		//ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f) fontIndex = 0;

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex].DrawClippingBoxSprite(
			{ x + i * fontWidth * scale,y,0 },
			scale,
			{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
			{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
			{ 1.0f,1.0f,1.0f,1.0f },
			texhandle);

		//添え字用変数をインクリメント
		spriteIndex++;
	}

	//変数の中身表示用------------------------
	if (variable != NULL)
	{
		float number = variable;
		int printNumber[13] = { };

		printNumber[0] = number / 100000;
		number -= (float)((int)number / (int)100000 * 100000);

		printNumber[1] = number / 10000;
		number -= (float)((int)number / 10000 * 10000);

		printNumber[2] = number / 1000;
		number -= (float)((int)number / 1000 * 1000);

		printNumber[3] = number / 100;
		number -= (float)((int)number / 100 * 100);

		printNumber[4] = number / 10;
		number -= (float)((int)number / 10 * 10);

		printNumber[5] = number / 1;
		number -= (float)((int)number / 1);

//小数点以下の表示
		printNumber[6] = -2;//小数点(0より２個前)

		number *= 1000000;

		printNumber[7] = number / 100000;
		number -= (int)number / (int)100000 * 100000;

		printNumber[8] = number / 10000;
		number -= (int)number / (int)10000 * 10000;

		printNumber[9] = number / 1000;
		number -= (int)number / (int)1000 * 1000;

		printNumber[10] = number / 100;
		number -= (int)number / (int)100 * 100;

		printNumber[11] = number / 10;
		number -= (int)number / (int)10 * 10;

		printNumber[12] = number / 1;
		number -= (int)number / (int)1;


		for (int i = 0; i < _countof(printNumber); i++)
		{
			//ASCIIコードの1段分飛ばした番号を計算
			int fontIndex = printNumber[i] + 16;

			int fontIndexY = fontIndex / fontLineCount;
			int fontIndexX = fontIndex % fontLineCount;

			//座標計算
			sprites[spriteIndex].DrawClippingBoxSprite(
				{ x + (i + text.size()) * fontWidth * scale,y,0 },
				scale,
				{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
				{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
				{ 1.0f,1.0f,1.0f,1.0f },
				texhandle);

			//添え字用変数をインクリメント
			spriteIndex++;
		}
	}

	spriteIndex = 0;
}
