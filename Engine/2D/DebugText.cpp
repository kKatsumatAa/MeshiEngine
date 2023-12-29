#include "DebugText.h"
using namespace std;


void DebugText::Print(const std::string& text, float x, float y, int32_t variable, float scale)
{
	for (int32_t i = 0; i < text.size(); i++)
	{
		if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

		const unsigned char& CHARACTER = text[i];

		//添え字として使うため
		int32_t fontIndex = CHARACTER + S_FONT_INDEX_OFFSET_;
		if (CHARACTER >= S_JUDGE_CHARACTER_) {
			fontIndex = 0;
		}

		int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
		int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


		leftTop_[spriteIndex_] = { x + S_FONT_WIDTH_ * scale * i,y };
		scale_[spriteIndex_] = scale;
		UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
		UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

		spriteIndex_++;
	}
	//変数の中身表示用------------------------
	if (variable != S_INVALID_TMP_)
	{
		bool isMinus = false;

		int32_t number = variable;
		int32_t printNumber[S_PRINT_NUMS_NUM_] = { };

		//マイナス
		if (number < 0)
		{
			isMinus = true;
			number = -number;
		}

		printNumber[0] = number / S_DIGIT_RATE_1_;
		number = number % S_DIGIT_RATE_1_;

		printNumber[1] = number / S_DIGIT_RATE_2_;
		number = number % S_DIGIT_RATE_2_;

		printNumber[2] = number / S_DIGIT_RATE_3_;
		number = number % S_DIGIT_RATE_3_;

		printNumber[3] = number / S_DIGIT_RATE_4_;
		number = number % S_DIGIT_RATE_4_;

		printNumber[4] = number / S_DIGIT_RATE_5_;
		number = number % S_DIGIT_RATE_5_;

		printNumber[5] = number;


		for (int32_t i = 0; i < _countof(printNumber); i++)
		{
			//最大文字数を超えたら
			if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

			//マイナスの時
			if (isMinus)
			{
				int32_t fontIndex = '-' + S_FONT_INDEX_OFFSET_;

				int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
				int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


				leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
				scale_[spriteIndex_] = scale;
				UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
				UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

				// 添え字用変数をインクリメント
				spriteIndex_++;

				isMinus = false;

				continue;
			}

			//ASCIIコードの1段分飛ばした番号を計算
			int32_t fontIndex = printNumber[i] + S_STEP_LENGTH_;

			int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
			int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;

			//
			leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
			scale_[spriteIndex_] = scale;
			UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
			UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

			//添え字用変数をインクリメント
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

		int32_t fontIndex = CHARACTER + S_FONT_INDEX_OFFSET_;
		if (CHARACTER >= S_JUDGE_CHARACTER_) fontIndex = 0;

		int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
		int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


		leftTop_[spriteIndex_] = { x + S_FONT_WIDTH_ * scale * i,y };
		scale_[spriteIndex_] = scale;
		UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
		UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

		spriteIndex_++;
	}
	//変数の中身表示用------------------------
	if (variable != (float)S_INVALID_TMP_)
	{
		bool isMinus = false;

		float number = variable;
		int32_t printNumber[S_PRINT_FLOAT_NUMS_NUM_] = { };

		//マイナス
		if (number < 0)
		{
			isMinus = true;
			number = -number;
		}

		printNumber[0] = (int)(number / S_DIGIT_RATE_1_);
		number -= (float)((int)number / S_DIGIT_RATE_1_ * S_DIGIT_RATE_1_);

		printNumber[1] = (int)(number / S_DIGIT_RATE_2_);
		number -= (float)((int)number / S_DIGIT_RATE_2_ * S_DIGIT_RATE_2_);

		printNumber[2] = (int)(number / S_DIGIT_RATE_3_);
		number -= (float)((int)number / S_DIGIT_RATE_3_ * S_DIGIT_RATE_3_);

		printNumber[3] = (int)(number / S_DIGIT_RATE_4_);
		number -= (float)((int)number / S_DIGIT_RATE_4_ * S_DIGIT_RATE_4_);

		printNumber[4] = (int)(number / S_DIGIT_RATE_5_);
		number -= (float)((int)number / S_DIGIT_RATE_5_ * S_DIGIT_RATE_5_);

		printNumber[5] = (int)(number);
		number -= (float)((int)number);

		//小数点以下の表示
		printNumber[6] = DECIMAL_POINT_;//小数点(0より２個前)

		number *= S_FLOAT_DIGIT_RATE_;

		printNumber[7] = (int)(number / S_DIGIT_RATE_1_);
		number -= (int)number / (int)S_DIGIT_RATE_1_ * S_DIGIT_RATE_1_;

		printNumber[8] = (int)(number / S_DIGIT_RATE_2_);
		number -= (int)number / (int)S_DIGIT_RATE_2_ * S_DIGIT_RATE_2_;

		printNumber[9] = (int)(number / S_DIGIT_RATE_3_);
		number -= (int)number / (int)S_DIGIT_RATE_3_ * S_DIGIT_RATE_3_;

		printNumber[10] = (int)(number / S_DIGIT_RATE_4_);
		number -= (int)number / (int)S_DIGIT_RATE_4_ * S_DIGIT_RATE_4_;

		printNumber[11] = (int)(number / S_DIGIT_RATE_5_);
		number -= (int)number / (int)S_DIGIT_RATE_5_ * S_DIGIT_RATE_5_;

		printNumber[12] = (int)(number);
		number -= (int)number;


		for (int32_t i = 0; i < _countof(printNumber); i++)
		{
			//最大文字数を超えたら
			if (spriteIndex_ >= S_MAX_CHAR_COUNT_) break;

			//マイナスの時
			if (isMinus)
			{
				int32_t fontIndex = '-' + S_FONT_INDEX_OFFSET_;

				int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
				int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;


				leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
				scale_[spriteIndex_] = scale;
				UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
				UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

				//添え字用変数をインクリメント
				spriteIndex_++;

				isMinus = false;

				continue;
			}

			//ASCIIコードの1段分飛ばした番号を計算
			int32_t fontIndex = printNumber[i] + S_STEP_LENGTH_;

			int32_t fontIndexY = fontIndex / S_FONT_LINE_COUNT_;
			int32_t fontIndexX = fontIndex % S_FONT_LINE_COUNT_;

			//
			leftTop_[spriteIndex_] = { x + (i + text.size()) * S_FONT_WIDTH_ * scale,y };
			scale_[spriteIndex_] = scale;
			UVleftTop_[spriteIndex_] = { (float)(fontIndexX) / (float)((S_FONT_INDEX_MAX_X_)) + UV_TOP_OFFSET_,(float)(fontIndexY) / (float)((S_FONT_INDEX_MAX_Y_)) + UV_TOP_OFFSET_ };
			UVlength_[spriteIndex_] = { (float)S_FONT_WIDTH_ / (float)(S_FONT_WIDTH_ * (S_FONT_INDEX_MAX_X_)) ,(float)S_FONT_HEIGHT_ / (float)(S_FONT_HEIGHT_ * (S_FONT_INDEX_MAX_Y_)) };

			//添え字用変数をインクリメント
			spriteIndex_++;
		}
	}
}

void DebugText::DrawAll(uint64_t texhandle, Camera2D* camera)
{
	for (int32_t i = 0; i < spriteIndex_; i++)
	{
		sprites_[i].SetTrans({ leftTop_[i].x,leftTop_[i].y ,0 });
		sprites_[i].SetScale({ scale_[i],scale_[i],1.0f });
		sprites_[i].SetTexHandle(texhandle);

		sprites_[i].DrawClippingBoxSprite(camera, UVleftTop_[i], UVlength_[i]);
	}

	spriteIndex_ = 0;
}
