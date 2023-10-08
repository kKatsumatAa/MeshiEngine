#pragma once
#include"Vec2.h"

const float PI = 3.1415f;

class Vec3
{
private:


public:
	float x;
	float y;
	float z;

	Vec3()
		:x(0), y(0), z(0)
	{}
	Vec3(float x, float y, float z)
		:x(x), y(y), z(z)
	{}
	Vec3(const Vec2& xy, float z)
		:x(xy.x), y(xy.y), z(z)
	{}
	Vec3(float x, const Vec2& yz)
		:x(x), y(yz.x), z(yz.y)
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