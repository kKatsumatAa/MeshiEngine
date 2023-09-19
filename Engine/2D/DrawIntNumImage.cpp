#include "DrawIntNumImage.h"
#include <math.h>

void DrawIntNumImage::Initialize(uint64_t texhandle)
{
	//�]��
	numRemainder_ = 0;
	//���̌�
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
	//�ŏ��ɐ����S�������
	numRemainder_ = num;

	//�����n�܂����炻������S���`�悷��p
	isStartDigit_ = false;

	//�����̐����J�E���g
	numCount_ = 0;

	for (int32_t i = _countof(numImages_) - 1; i >= 0; i--)
	{
		//���̌��̐������o��
		numDigit_ = numRemainder_ / (int)std::pow(10, i);
		//�]����o���Ď��̌��Ŏg��
		numRemainder_ = numRemainder_ % (int)std::pow(10, i);

		//���̌������邩�A�O�̌�����������,0��������A0�ꌅ��������
		if (numDigit_ > 0 || isStartDigit_ == true ||
			(numDigit_ == 0 && numRemainder_ == 0 && i == _countof(numImages_) - 1))
		{
			isStartDigit_ = true;

			if ((numDigit_ == 0 && numRemainder_ == 0 && i == _countof(numImages_) - 1))
			{
				isStartDigit_ = false;
			}

			numImages_[i].num = numDigit_;
			numImages_[i].pos = pos;
			numImages_[i].sizeUV = sizeUV;
			numImages_[i].scale = scale;
			numImages_[i].color = color;
			numImages_[i].numImageSize = numImageSize;
			numImages_[i].isTrue = true;

			if (numCount_ < i)
			{
				numCount_ = i;
			}
		}
		else
		{
			numImages_[i].isTrue = false;
		}
	}
}

void DrawIntNumImage::Draw(Camera2D* camera)
{
	//��ԑ傫��������X�^�[�g
	for (int32_t i = numCount_; i >= 0; i--)
	{
		if (numImages_[i].isTrue)
		{
			//���̐����l�����āA������W
			Vec2 pos = { numImages_[i].pos.x + numImages_[i].numImageSize.x * ((float)(numCount_) - (float)i)
				* numImages_[i].scale
				,numImages_[i].pos.y };



			numImages_[i].obj.SetTrans({ pos.x, pos.y ,0 });
			numImages_[i].obj.SetScale({ numImages_[i].scale, numImages_[i].scale,1.0f });

			numImages_[i].obj.DrawClippingBoxSprite(camera, { numImages_[i].num * numImages_[i].sizeUV.x,0 },
				{ numImages_[i].sizeUV.x,numImages_[i].sizeUV.y }, texhandle_, numImages_[i].color, true);
		}
	}
}
