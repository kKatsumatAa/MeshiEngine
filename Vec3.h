#pragma once
#include<DirectXMath.h>

class Vec3
{
private:


public:
	float x;
	float y;
	float z;
	float w;

	Vec3()
		:x(0), y(0), z(0), w(0)
	{}
	Vec3(const float x, const float y, const float z, const float w)
		:x(x), y(y), z(z), w(w)
	{}

	//便利関数
	float GetLength() const;
	void Normalized();
	const Vec3 GetNormalized();
	Vec3 Cross(const Vec3& other) const;
	float Dot(const Vec3& other) const;

	//オバロ演算子
	Vec3 operator+(const Vec3& other);
	Vec3& operator+=(const Vec3& other);
	Vec3 operator-(const Vec3& other);
	Vec3& operator-=(const Vec3& other);
	Vec3 operator*(const float& other);
	Vec3& operator*=(const float& other);
	Vec3 operator/(const float& other);
	Vec3& operator/=(const float& other);
};

//二項演算子オーバーロード
const Vec3 operator +(const Vec3& v1, const Vec3& v2);
const Vec3 operator -(const Vec3& v1, const Vec3& v2);
const Vec3 operator *(const Vec3& v, float s);
const Vec3 operator *(float s, const Vec3& v);
const Vec3 operator /(const Vec3& v, float s);