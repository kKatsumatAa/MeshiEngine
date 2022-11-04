#include "DebugText.h"
using namespace std;


void DebugText::Print(const std::string& text, float x, float y, int variable, float scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (spriteIndex >= maxCharCount) break;

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) fontIndex = 0;

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;


		leftTop[spriteIndex] = { x + fontWidth * scale * i,y,0 };
		this->scale[spriteIndex] = scale;
		UVleftTop[spriteIndex] = { (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f };
		UVlength[spriteIndex] = { (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) };

		spriteIndex++;
	}
	//�ϐ��̒��g�\���p------------------------
	if (variable != 114514)
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
			//�ő啶�����𒴂�����
			if (spriteIndex >= maxCharCount) break;

			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
			int fontIndex = printNumber[i] + 16;
			//if (character >= 0x7f) fontIndex = 0;

			int fontIndexY = fontIndex / fontLineCount;
			int fontIndexX = fontIndex % fontLineCount;

			//
			leftTop[spriteIndex] = { x + (i + text.size()) * fontWidth * scale,y,0 };
			this->scale[spriteIndex] = scale;
			UVleftTop[spriteIndex] = { (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f };
			UVlength[spriteIndex] = { (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) };

			//�Y�����p�ϐ����C���N�������g
			spriteIndex++;
		}
	}
}

void DebugText::Printf(const std::string& text, float x, float y, float variable, float scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (spriteIndex >= maxCharCount) break;

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) fontIndex = 0;

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;


		leftTop[spriteIndex] = { x + fontWidth * scale * i,y,0 };
		this->scale[spriteIndex] = scale;
		UVleftTop[spriteIndex] = { (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f };
		UVlength[spriteIndex] = { (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) };

		spriteIndex++;
	}
	//�ϐ��̒��g�\���p------------------------
	if (variable != 114514.0f)
	{
		float number = variable;
		int printNumber[13] = { };

		printNumber[0] = (int)(number / 100000);
		number -= (float)((int)number / 100000 * 100000);

		printNumber[1] = (int)(number / 10000);
		number -= (float)((int)number / 10000 * 10000);

		printNumber[2] = (int)(number / 1000);
		number -= (float)((int)number / 1000 * 1000);

		printNumber[3] = (int)(number / 100);
		number -= (float)((int)number / 100 * 100);

		printNumber[4] = (int)(number / 10);
		number -= (float)((int)number / 10 * 10);

		printNumber[5] = (int)(number / 1);
		number -= (float)((int)number / 1);

		//�����_�ȉ��̕\��
		printNumber[6] = -2;//�����_(0���Q�O)

		number *= 1000000;

		printNumber[7] = (int)(number / 100000);
		number -= (int)number / (int)100000 * 100000;

		printNumber[8] = (int)(number / 10000);
		number -= (int)number / (int)10000 * 10000;

		printNumber[9] = (int)(number / 1000);
		number -= (int)number / (int)1000 * 1000;

		printNumber[10] = (int)(number / 100);
		number -= (int)number / (int)100 * 100;

		printNumber[11] = (int)(number / 10);
		number -= (int)number / (int)10 * 10;

		printNumber[12] = (int)(number / 1);
		number -= (int)number / (int)1;


		for (int i = 0; i < _countof(printNumber); i++)
		{
			//�ő啶�����𒴂�����
			if (spriteIndex >= maxCharCount) break;

			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
			int fontIndex = printNumber[i] + 16;
			//if (character >= 0x7f) fontIndex = 0;

			int fontIndexY = fontIndex / fontLineCount;
			int fontIndexX = fontIndex % fontLineCount;

			//
			leftTop[spriteIndex] = { x + (i + text.size()) * fontWidth * scale,y,0 };
			this->scale[spriteIndex] = scale;
			UVleftTop[spriteIndex] = { (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f };
			UVlength[spriteIndex] = { (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) };

			//�Y�����p�ϐ����C���N�������g
			spriteIndex++;
		}
	}
}

void DebugText::DrawAll(UINT64 texhandle)
{
	for (int i = 0; i < spriteIndex; i++)
	{
		sprites[i].DrawClippingBoxSprite(leftTop[i], scale[i], UVleftTop[i], UVlength[i],
			{ 1,1,1,1 }, texhandle);
	}

	spriteIndex = 0;
}

//void DebugText::Print(const std::string& text, float x, float y, UINT64 texhandle, int variable, float scale)
//{
//	for (int i = 0; i < text.size(); i++)
//	{
//		//�ő啶�����𒴂�����
//		if (spriteIndex >= maxCharCount) break;
//
//		//1���������o��
//		const unsigned char& character = text[i];
//
//		//ASCII�R�[�h��2�i����΂����ԍ����v�Z
//		int fontIndex = character - 32;
//		if (character >= 0x7f) fontIndex = 0;
//
//		int fontIndexY = fontIndex / fontLineCount;
//		int fontIndexX = fontIndex % fontLineCount;
//
//		//���W�v�Z
//		sprites[spriteIndex].DrawClippingBoxSprite(
//			{ x + i * fontWidth * scale,y,0 },
//			scale,
//			{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
//			{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
//			{ 1.0f,1.0f,1.0f,1.0f },
//			texhandle);
//
//		//�Y�����p�ϐ����C���N�������g
//		spriteIndex++;
//	}
//
//	//�ϐ��̒��g�\���p------------------------
//	if (variable != NULL)
//	{
//		int number = variable;
//		int printNumber[6] = { };
//
//		printNumber[0] = number / 100000;
//		number = number % 100000;
//
//		printNumber[1] = number / 10000;
//		number = number % 10000;
//
//		printNumber[2] = number / 1000;
//		number = number % 1000;
//
//		printNumber[3] = number / 100;
//		number = number % 100;
//
//		printNumber[4] = number / 10;
//		number = number % 10;
//
//		printNumber[5] = number;
//
//
//		for (int i = 0; i < _countof(printNumber); i++)
//		{
//			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
//			int fontIndex = printNumber[i] + 16;
//			//if (character >= 0x7f) fontIndex = 0;
//
//			int fontIndexY = fontIndex / fontLineCount;
//			int fontIndexX = fontIndex % fontLineCount;
//
//			//���W�v�Z
//			sprites[spriteIndex].DrawClippingBoxSprite(
//				{ x + (i + text.size()) * fontWidth * scale,y,0 },
//				scale,
//				{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
//				{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
//				{ 1.0f,1.0f,1.0f,1.0f },
//				texhandle);
//
//			//�Y�����p�ϐ����C���N�������g
//			spriteIndex++;
//		}
//	}
//
//	spriteIndex = 0;
//}
//
//void DebugText::Print(const std::string& text, float x, float y, UINT64 texhandle, const float& variable, float scale)
//{
//	for (int i = 0; i < text.size(); i++)
//	{
//		//�ő啶�����𒴂�����
//		if (spriteIndex >= maxCharCount) break;
//
//		//1���������o��
//		const unsigned char& character = text[i];
//
//		//ASCII�R�[�h��2�i����΂����ԍ����v�Z
//		int fontIndex = character - 32;
//		if (character >= 0x7f) fontIndex = 0;
//
//		int fontIndexY = fontIndex / fontLineCount;
//		int fontIndexX = fontIndex % fontLineCount;
//
//		//���W�v�Z
//		sprites[spriteIndex].DrawClippingBoxSprite(
//			{ x + i * fontWidth * scale,y,0 },
//			scale,
//			{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
//			{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
//			{ 1.0f,1.0f,1.0f,1.0f },
//			texhandle);
//
//		//�Y�����p�ϐ����C���N�������g
//		spriteIndex++;
//	}
//
//	//�ϐ��̒��g�\���p------------------------
//	if (variable != NULL)
//	{
//		float number = variable;
//		int printNumber[13] = { };
//
//		printNumber[0] = (int)(number / 100000);
//		number -= (float)((int)number / 100000 * 100000);
//
//		printNumber[1] = (int)(number / 10000);
//		number -= (float)((int)number / 10000 * 10000);
//
//		printNumber[2] = (int)(number / 1000);
//		number -= (float)((int)number / 1000 * 1000);
//
//		printNumber[3] = (int)(number / 100);
//		number -= (float)((int)number / 100 * 100);
//
//		printNumber[4] = (int)(number / 10);
//		number -= (float)((int)number / 10 * 10);
//
//		printNumber[5] = (int)(number / 1);
//		number -= (float)((int)number / 1);
//
//		//�����_�ȉ��̕\��
//		printNumber[6] = -2;//�����_(0���Q�O)
//
//		number *= 1000000;
//
//		printNumber[7] = (int)(number / 100000);
//		number -= (int)number / (int)100000 * 100000;
//
//		printNumber[8] = (int)(number / 10000);
//		number -= (int)number / (int)10000 * 10000;
//
//		printNumber[9] = (int)(number / 1000);
//		number -= (int)number / (int)1000 * 1000;
//
//		printNumber[10] = (int)(number / 100);
//		number -= (int)number / (int)100 * 100;
//
//		printNumber[11] = (int)(number / 10);
//		number -= (int)number / (int)10 * 10;
//
//		printNumber[12] = (int)(number / 1);
//		number -= (int)number / (int)1;
//
//
//		for (int i = 0; i < _countof(printNumber); i++)
//		{
//			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
//			int fontIndex = printNumber[i] + 16;
//
//			int fontIndexY = fontIndex / fontLineCount;
//			int fontIndexX = fontIndex % fontLineCount;
//
//			//���W�v�Z
//			sprites[spriteIndex].DrawClippingBoxSprite(
//				{ x + (i + text.size()) * fontWidth * scale,y,0 },
//				scale,
//				{ (float)(fontIndexX) / (float)((fontIndexMaxX)) - 0.003f,(float)(fontIndexY) / (float)((fontIndexMaxY)) - 0.003f },
//				{ (float)fontWidth / (float)(fontWidth * (fontIndexMaxX)) ,(float)fontHeight / (float)(fontHeight * (fontIndexMaxY)) },
//				{ 1.0f,1.0f,1.0f,1.0f },
//				texhandle);
//
//			//�Y�����p�ϐ����C���N�������g
//			spriteIndex++;
//		}
//	}
//
//	spriteIndex = 0;
//}
