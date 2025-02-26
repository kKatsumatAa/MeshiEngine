/*
* @file DrawIntNumImage.h
* @brief 数字を画像を使って画面に表示
*/

#pragma once
#include"Sprite.h"

class DrawIntNumImage
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

private:
	struct NumImage
	{
		Sprite sprite;
		Vec4 color;
		Vec2 sizeUV;
		Vec2 pos;
		int32_t num;
		float scale;
		Vec2 numImageSize;
		bool isTrue;
	};

public:
	static const int8_t DIGIT_ = 10;

private:
	NumImage numImages_[6];
	//余り
	int32_t numRemainder_;
	//その桁
	int32_t numDigit_;

	uint64_t texhandle_;

	bool isStartDigit_ = false;

	int32_t numCount_ = 0;


public:
	//初期化
	void Initialize(uint64_t texhandle);
	//数字をセット
	void SetNum(int32_t num,const Vec2& pos, const Vec2& sizeUV, const Vec2& numImageSize, float scale, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f });
	//描画
	void Draw(Camera2D* camera);
};

