#include "Vec4.h"

float Vec4::GetLength() const
{
	return sqrtf(x_ * x_ + y_ * y_ + z_ * z_);
}

void Vec4::Normalized()
{
	*this /= GetLength();
}

Vec4 Vec4::GetNormalized() const
{
	Vec4 v;
	v = *this / GetLength();

	return v;
}

Vec4 Vec4::Cross(const Vec4& other)const
{
	Vec4 v;

	v.x_ = y_ * other.z_ - z_ * other.y_;
	v.y_ = z_ * other.x_ - x_ * other.z_;
	v.z_ = x_ * other.y_ - y_ * other.x_;

	return v;
}

float Vec4::Dot(const Vec4& other)const
{
	return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}
//--------------------------------------------------------------


Vec4 Vec4::operator+(const Vec4& other)
{
	Vec4 v = *this;
	v.x_ += other.x_;
	v.y_ += other.y_;
	v.z_ += other.z_;

	return Vec4(v);
}

Vec4& Vec4::operator+=(const Vec4& other)
{
	x_ += other.x_;
	y_ += other.y_;
	z_ += other.z_;

	return *this;
}

Vec4 Vec4::operator-(const Vec4& other)
{
	Vec4 v = *this;
	v.x_ -= other.x_;
	v.y_ -= other.y_;
	v.z_ -= other.z_;

	return Vec4(v);
}

Vec4& Vec4::operator-=(const Vec4& other)
{
	x_ -= other.x_;
	y_ -= other.y_;
	z_ -= other.z_;

	return *this;
}

Vec4 Vec4::operator*(float other)
{
	Vec4 v = *this;
	v.x_ *= other;
	v.y_ *= other;
	v.z_ *= other;

	return Vec4(v);
}

Vec4& Vec4::operator*=(float other)
{
	x_ *= other;
	y_ *= other;
	z_ *= other;

	return *this;
}

Vec4 Vec4::operator/(float other)
{
	Vec4 v = *this;
	v.x_ /= other;
	v.y_ /= other;
	v.z_ /= other;

	return Vec4(v);
}

Vec4& Vec4::operator/=(float other)
{
	x_ /= other;
	y_ /= other;
	z_ /= other;

	return *this;
}


//-------------------------------------

const Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
	Vec4 temp(v1);
	return temp + v2;
}

const Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
	Vec4 temp(v1);
	return temp - v2;
}

const Vec4 operator*(const Vec4& v, float s)
{
	Vec4 temp(v);
	return temp * s;
}

const Vec4 operator*(float s, const Vec4& v)
{
	Vec4 temp(v);
	return temp * s;
}

const Vec4 operator/(const Vec4& v, float s)
{
	Vec4 temp(v);
	return temp / s;
}
