/*
* @file DebugText.h
* @brief デバッグ用の文字画像の処理
*/

#pragma once
#include"Sprite.h"

class DebugText
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static const int32_t S_INVALID_TMP_ = 114514;
	static const int32_t S_FONT_INDEX_OFFSET_ = -32;
	static const int32_t S_JUDGE_CHARACTER_ = 0x7f;
	const float UV_TOP_OFFSET_ = -0.003f;
	static const int32_t S_PRINT_NUMS_NUM_ = 6;
	static const int32_t S_PRINT_FLOAT_NUMS_NUM_ = 13;
	//一段飛ばしたときの数
	static const int32_t S_STEP_LENGTH_ = 16;
	//桁ごとの数字を得るための
	static const int32_t S_FLOAT_DIGIT_RATE_ = 1000000;
	static const int32_t S_DIGIT_RATE_1_ = 100000;
	static const int32_t S_DIGIT_RATE_2_ = 10000;
	static const int32_t S_DIGIT_RATE_3_ = 1000;
	static const int32_t S_DIGIT_RATE_4_ = 100;
	static const int32_t S_DIGIT_RATE_5_ = 10;
	//小数点
	static const int32_t DECIMAL_POINT_ = -2;

private:
	static const int32_t S_MAX_CHAR_COUNT_ = 256;
	static const int32_t S_FONT_WIDTH_ = 9;
	static const int32_t S_FONT_HEIGHT_ = 18;
	static const int32_t S_FONT_LINE_COUNT_ = 14;
	static const int32_t S_FONT_INDEX_MAX_X_ = 14;
	static const int32_t S_FONT_INDEX_MAX_Y_ = 7;

	Sprite sprites_[S_MAX_CHAR_COUNT_];
	Vec2 leftTop_[S_MAX_CHAR_COUNT_];
	float scale_[S_MAX_CHAR_COUNT_];
	XMFLOAT2 UVleftTop_[S_MAX_CHAR_COUNT_];
	XMFLOAT2 UVlength_[S_MAX_CHAR_COUNT_];

	//添え字用の変数
	int32_t spriteIndex_ = 0;

public:
	//描画文字セット
	void Print(const std::string& text, float x, float y, int32_t variable = 114514, float scale = 1.0f);
	//描画文字セット（float）
	void Printf(const std::string& text, float x, float y, float variable = 114514.0f, float scale = 1.0f);

	//全て描画
	void DrawAll(uint64_t texhandle, Camera2D* camera);
};

