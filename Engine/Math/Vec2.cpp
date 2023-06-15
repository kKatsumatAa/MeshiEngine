#include "Vec2.h"
#include <DirectXMath.h>


float Vec2::GetLength() const
{
	return sqrtf(x_ * x_ + y_ * y_);
}

void Vec2::Normalized()
{
	float len = GetLength();
	if (len > 0)
	{
		*this /= GetLength();
	}
}

Vec2 Vec2::GetNormalize()
{
	float len = GetLength();
	if (len > 0) { return *this / len; }

	return *this;
}

float Vec2::Cross(const Vec2& other)const
{
	return x_ * other.y_ - y_ * other.x_;
}

float Vec2::Dot(const Vec2& other)const
{
	return x_ * other.x_ + y_ * other.y_;
}



//-----------------------------------------------------------------
//演算子オーバーロード
Vec2 Vec2::operator-()
{
	return { -x_,-y_ };
}

Vec2& Vec2::operator-=(const Vec2& other)
{
	x_ -= other.x_;
	y_ -= other.y_;

	return *this;
}


Vec2 Vec2::operator+()
{
	return *this;
}

Vec2& Vec2::operator+=(const Vec2& other)
{
	x_ += other.x_;
	y_ += other.y_;

	return *this;
}



Vec2 Vec2::operator*(float other)
{
	Vec2 v;
	v.x_ = x_ * other;
	v.y_ = y_ * other;

	return v;
}

Vec2& Vec2::operator*=(float other)
{
	*this = *this * other;//thisはアドレスなので*つける

	return *this;
}



Vec2 Vec2::operator/(float other)
{
	Vec2 v;
	v.x_ = x_ / other;
	v.y_ = y_ / other;

	return v;
}

Vec2& Vec2::operator/=(float other)
{
	*this = *this / other;//thisはアドレスなので*つける

	return *this;
}




const Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
	Vec2 temp(v1);
	return temp += v2;
}

const Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	Vec2 temp(v1);
	return temp -= v2;
}

const Vec2 operator*(const Vec2& v, float s)
{
	Vec2 temp(v);
	return temp *= s;
}

const Vec2 operator*(float s, const Vec2& v)
{
	Vec2 temp(v);
	return temp *= s;
}

const Vec2 operator/(const Vec2& v, float s)
{
	Vec2 temp(v);
	return temp /= s;
}