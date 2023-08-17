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

		//���̌������邩�A�O�̌�����������,0��������
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
		//��ԑ傫��������X�^�[�g
		for (int32_t i = numCount_ - 1; i >= 0; i--)
		{
			if (numImages_[i].isTrue)
			{
				//���̐����l�����āA���S���W����̋������o���A�^�̍��W�Ƃ���@�@�@�@�@�@�@�@�@�@//�傫��������X�^�[�g�Ȃ̂ŋt���ɂ��āA��������ׂ�i���W�j
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
