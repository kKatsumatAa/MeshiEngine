#include "DrawIntNumImage.h"
#include <math.h>

void DrawIntNumImage::Initialize(uint64_t texhandle)
{
	//余り
	numRemainder_ = 0;
	//その桁
	numDigit_ = 0;

	texhandle_ = texhandle;

	numCount_ = 0;

	for (int32_t i = _countof(numImages_) - 1; i >= 0; i--)
	{
		numImages_[i].isTrue = false;
	}
}

void DrawIntNumImage::SetNum(int32_t num, const Vec2& pos, const Vec2& sizeUV, const Vec2& numImageSize, float scale, const Vec4& color)
{
	//最初に数字全部入れる
	numRemainder_ = num;

	//桁が始まったらそこから全部描画する用
	isStartDigit_ = false;

	//数字の数をカウント
	numCount_ = 0;

	for (int32_t i = _countof(numImages_) - 1; i >= 0; i--)
	{
		//その桁の数字を出す
		numDigit_ = numRemainder_ / (int)std::pow(10, i);
		//余りを出して次の桁で使う
		numRemainder_ = numRemainder_ % (int)std::pow(10, i);

		//その桁があるか、前の桁があったら,0だったら
		if (numDigit_ > 0 || isStartDigit_ == true || i == 0)
		{
			isStartDigit_ = true;

			numImages_[i].num = numDigit_;
			numImages_[i].pos = pos;
			numImages_[i].sizeUV = sizeUV;
			numImages_[i].scale = scale;
			numImages_[i].color = color;
			numImages_[i].numImageSize = numImageSize;
			numImages_[i].isTrue = true;

			numCount_++;
		}
		else
		{
			numImages_[i].isTrue = false;
		}
	}
}

void DrawIntNumImage::Draw()
{
	if (isStartDigit_)
	{
		//一番大きい桁からスタート
		for (int32_t i = numCount_ - 1; i >= 0; i--)
		{
			if (numImages_[i].isTrue)
			{
				//桁の数も考慮して、中心座標からの距離を出し、真の座標とする　　　　　　　　　　//大きい桁からスタートなので逆順にして、左から並べる（座標）
				Vec2 pos = { numImages_[i].pos.x_ + numImages_[i].numImageSize.x_ / 2.0f * ((float)(numCount_ - 1) - (float)i - (float)numCount_ / 2.0f) * numImages_[i].scale
					+ (numImages_[i].numImageSize.x_ / 4.0f * numImages_[i].scale)
					,numImages_[i].pos.y_ };

				pos.x_ -= numImages_[i].numImageSize.x_ / 2.0f * numImages_[i].scale;
				pos.y_ -= numImages_[i].numImageSize.y_ / 2.0f * numImages_[i].scale;

				numImages_[i].obj.DrawClippingBoxSprite({ pos.x_,pos.y_ }, { numImages_[i].scale,numImages_[i].scale },
					{ numImages_[i].num * numImages_[i].sizeUV.x_,0 },
					{ numImages_[i].sizeUV.x_,numImages_[i].sizeUV.y_ }, numImages_[i].color, texhandle_, true);
			}
		}
	}
}
