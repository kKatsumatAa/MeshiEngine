#pragma once
#include<DirectXMath.h>

const float PI = 3.1415f;

class Vec3
{
private:


public:
	float x_;
	float y_;
	float z_;

	Vec3()
		:x_(0), y_(0), z_(0)
	{}
	Vec3(float x, float y, float z)
		:x_(x), y_(y), z_(z)
	{}

	//便利関数
	float GetLength() const;
	void Normalized();
	Vec3 GetNormalized() const;
	Vec3 Cross(const Vec3& other) const;
	float Dot(const Vec3& other) const;

	//オバロ演算子
	Vec3 operator+()const;
	Vec3& operator+=(const Vec3& other);
	Vec3 operator-()const;
	Vec3& operator-=(const Vec3& other);
	Vec3 operator*(float other);
	Vec3& operator*=(float other);
	Vec3 operator/(float other);
	Vec3& operator/=(float other);
};

//二項演算子オーバーロード
const Vec3 operator +(const Vec3& v1, const Vec3& v2);
const Vec3 operator -(const Vec3& v1, const Vec3& v2);
const Vec3 operator *(const Vec3& v, float s);
const Vec3 operator *(float s, const Vec3& v);
const Vec3 operator /(const Vec3& v, float s);