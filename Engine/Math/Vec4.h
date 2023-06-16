#pragma once
#include<DirectXMath.h>

class Vec4
{
private:


public:
	float x_;
	float y_;
	float z_;
	float w_;

	Vec4()
		:x_(0), y_(0), z_(0), w_(0)
	{}
	Vec4(float x, float y, float z, float w)
		:x_(x), y_(y), z_(z), w_(w)
	{}

	//便利関数
	float GetLength() const;
	void Normalized();
	Vec4 GetNormalized() const;
	Vec4 Cross(const Vec4& other) const;
	float Dot(const Vec4& other) const;

	//オバロ演算子
	Vec4 operator+(const Vec4& other);
	Vec4& operator+=(const Vec4& other);
	Vec4 operator-(const Vec4& other);
	Vec4& operator-=(const Vec4& other);
	Vec4 operator*(float other);
	Vec4& operator*=(float other);
	Vec4 operator/(float other);
	Vec4& operator/=(float other);
};

//二項演算子オーバーロード
const Vec4 operator +(const Vec4& v1, const Vec4& v2);
const Vec4 operator -(const Vec4& v1, const Vec4& v2);
const Vec4 operator *(const Vec4& v, float s);
const Vec4 operator *(float s, const Vec4& v);
const Vec4 operator /(const Vec4& v, float s);