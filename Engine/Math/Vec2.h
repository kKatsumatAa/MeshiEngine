#pragma once

class Vec2
{
public:
	float x_;
	float y_;

public:
	//�R���X�g���N�^
	Vec2()
		:x_(0), y_(0)
	{}

	Vec2(float x, float y)
		:x_(x), y_(y)
	{}

	//�����o�֐�
	float GetLength() const;
	void  Normalized();
	Vec2  GetNormalize();
	float Dot(const Vec2& v) const;
	float Cross(const Vec2& v) const;

	Vec2  operator -();//vec���m�̈����Z
	Vec2& operator -=(const Vec2& other);//�������Z�q(-)

	Vec2  operator +();
	Vec2& operator +=(const Vec2& other);//�������Z�q(+)

	Vec2  operator *(float other);
	Vec2& operator *=(float other);//�������Z�q(*)

	Vec2  operator /(float other);
	Vec2& operator /=(float other);//�������Z�q(/)
};

//�񍀉��Z�q�I�[�o�[���[�h
const Vec2 operator +(const Vec2& v1, const Vec2& v2);
const Vec2 operator -(const Vec2& v1, const Vec2& v2);
const Vec2 operator *(const Vec2& v, float s);
const Vec2 operator *(float s, const Vec2& v);
const Vec2 operator /(const Vec2& v, float s);