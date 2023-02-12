#include "DrawIntNumImage.h"
#include <math.h>

void DrawIntNumImage::Initialize(UINT64 texhandle)
{
	//余り
	numRemainder = 0;
	//その桁
	numDigit = 0;

	this->texhandle = texhandle;

	numCount = 0;

	for (int i = _countof(numImages) - 1; i >= 0; i--)
	{
		this->numImages[i].isTrue = false;
	}
}

void DrawIntNumImage::SetNum(int num, Vec2 pos, Vec2 sizeUV, Vec2 numImageSize, float scale, XMFLOAT4 color)
{
	//最初に数字全部入れる
	numRemainder = num;

	//桁が始まったらそこから全部描画する用
	isStartDigit = false;

	//数字の数をカウント
	numCount = 0;

	for (int i = _countof(numImages) - 1; i >= 0; i--)
	{
		//その桁の数字を出す
		numDigit = numRemainder / (int)std::pow(10, i);
		//余りを出して次の桁で使う
		numRemainder = numRemainder % (int)std::pow(10, i);

		//その桁があるか、前の桁があったら,0だったら
		if (numDigit > 0 || isStartDigit == true || i == 0)
		{
			isStartDigit = true;

			this->numImages[i].num = numDigit;
			this->numImages[i].pos = pos;
			this->numImages[i].sizeUV = sizeUV;
			this->numImages[i].scale = scale;
			this->numImages[i].color = color;
			this->numImages[i].numImageSize = numImageSize;
			this->numImages[i].isTrue = true;

			numCount++;
		}
		else
		{
			this->numImages[i].isTrue = false;
		}
	}
}

void DrawIntNumImage::Draw()
{
	if (isStartDigit)
	{
		//一番大きい桁からスタート
		for (int i = numCount - 1; i >= 0; i--)
		{
			if (this->numImages[i].isTrue)
			{
				//桁の数も考慮して、中心座標からの距離を出し、真の座標とする　　　　　　　　　　//大きい桁からスタートなので逆順にして、左から並べる（座標）
				Vec2 pos = { numImages[i].pos.x + numImages[i].numImageSize.x / 2.0f * ((float)(numCount - 1) - (float)i - (float)numCount / 2.0f) * numImages[i].scale
					+ (numImages[i].numImageSize.x / 4.0f * numImages[i].scale)
					,numImages[i].pos.y };

				pos.x -= numImages[i].numImageSize.x / 2.0f * numImages[i].scale;
				pos.y -= numImages[i].numImageSize.y / 2.0f * numImages[i].scale;

				numImages[i].obj.DrawClippingBoxSprite({ pos.x,pos.y,0 }, numImages[i].scale, { numImages[i].num * numImages[i].sizeUV.x,0 },
					{ numImages[i].sizeUV.x,numImages[i].sizeUV.y }, numImages[i].color, texhandle, true);
			}
		}
	}
}
