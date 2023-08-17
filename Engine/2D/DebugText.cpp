#include "DebugText.h"
using namespace std;


void DebugText::Print(const std::string& text, float x, float y, int32_t variable, float scale)
{
	for (int32_t i = 0; i < text.size(); i++)
	{
		if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

		const unsigned char& CHARACTER = text[i];

		int32_t fontIndex = CHARACTER - 32;
		if (CHARACTER >= 0x7f) fontIndex = 0;

		int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
		int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


		leftTop_[spriteIndex_] = { x + S_FONT_WIDTH_ * scale * i,y };
		scale_[spriteIndex_] = scale;
		UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
		UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

		spriteIndex_++;
	}
	//�ϐ��̒��g�\���p------------------------
	if (variable != 114514)
	{
		bool isMinus = false;

		int32_t number = variable;
		int32_t printNumber[6] = { };

		//�}�C�i�X
		if (number < 0)
		{
			isMinus = true;
			number *= -1;
		}

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


		for (int32_t i = 0; i < _countof(printNumber); i++)
		{
			//�ő啶�����𒴂�����
			if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

			//�}�C�i�X�̎�
			if (isMinus)
			{
				int32_t fontIndex = '-' - 32;

				int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
				int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


				leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
				scale_[spriteIndex_] = scale;
				UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
				UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

				// �Y�����p�ϐ����C���N�������g
				spriteIndex_++;

				isMinus = false;

				continue;
			}

			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
			int32_t fontIndex = printNumber[i] + 16;
			//if (character >= 0x7f) fontIndex = 0;

			int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
			int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;

			//
			leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
			scale_[spriteIndex_] = scale;
			UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
			UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

			//�Y�����p�ϐ����C���N�������g
			spriteIndex_++;
		}
	}
}

void DebugText::Printf(const std::string& text, float x, float y, float variable, float scale)
{
	for (int32_t i = 0; i < text.size(); i++)
	{
		if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

		const unsigned char& CHARACTER = text[i];

		int32_t fontIndex = CHARACTER - 32;
		if (CHARACTER >= 0x7f) fontIndex = 0;

		int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
		int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


		leftTop_[spriteIndex_] = { x + S_FONT_WIDTH_ * scale * i,y };
		scale_[spriteIndex_] = scale;
		UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
		UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

		spriteIndex_++;
	}
	//�ϐ��̒��g�\���p------------------------
	if (variable != 114514.0f)
	{
		bool isMinus = false;

		float number = variable;
		int32_t printNumber[13] = { };

		//�}�C�i�X
		if (number < 0)
		{
			isMinus = true;
			number *= -1;
		}

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


		for (int32_t i = 0; i < _countof(printNumber); i++)
		{
			//�ő啶�����𒴂�����
			if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

			//�}�C�i�X�̎�
			if (isMinus)
			{
				int32_t fontIndex = '-' - 32;

				int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
				int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


				leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
				scale_[spriteIndex_] = scale;
				UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
				UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

				//�Y�����p�ϐ����C���N�������g
				spriteIndex_++;

				isMinus = false;

				continue;
			}

			//ASCII�R�[�h��1�i����΂����ԍ����v�Z
			int32_t fontIndex = printNumber[i] + 16;
			//if (character >= 0x7f) fontIndex = 0;

			int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
			int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;

			//
			leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
			scale_[spriteIndex_] = scale;
			UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) - 0.003f,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) - 0.003f };
			UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

			//�Y�����p�ϐ����C���N�������g
			spriteIndex_++;
		}
	}
}

void DebugText::DrawAll(uint64_t texhandle)
{
	for (int32_t i = 0; i < spriteIndex_; i++)
	{
		sprites_[i].DrawClippingBoxSprite(leftTop_[i], { scale_[i],scale_[i] }, UVleftTop_[i], UVlength_[i],
			{ 1.0f,1.0f,1.0f,1.0f }, texhandle);
	}

	spriteIndex_ = 0;
}
