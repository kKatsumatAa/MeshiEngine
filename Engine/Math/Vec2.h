/*
* @file Vec2.h
* @brief 二次元ベクトル
*/

#pragma once

#pragma warning(push)
//c5267警告を無視
#pragma warning(disable:5267)
#include <DirectXMath.h>
#pragma warning(pop)

class Vec2
{
public:
	float x;
	float y;

public:
	//コンストラクタ
	Vec2()
		:x(0), y(0)
	{}

	Vec2(float x, float y)
		:x(x), y(y)
	{}

	//メンバ関数
	float GetLength() const;
	void  Normalized();
	Vec2  GetNormalize() const;
	float Dot(const Vec2& v) const;
	float Cross(const Vec2& v) const;

	Vec2  operator -()const;//vec同士の引き算
	Vec2& operator -=(const Vec2& other);//複合演算子(-)

	Vec2  operator +()const;
	Vec2& operator +=(const Vec2& other);//複合演算子(+)

	Vec2  operator *(float other);
	Vec2& operator *=(float other);//複合演算子(*)

	Vec2  operator /(float other);
	Vec2& operator /=(float other);//複合演算子(/)
};

//二項演算子オーバーロード
const Vec2 operator +(const Vec2& v1, const Vec2& v2);
const Vec2 operator -(const Vec2& v1, const Vec2& v2);
const Vec2 operator *(const Vec2& v, float s);
const Vec2 operator *(float s, const Vec2& v);
const Vec2 operator /(const Vec2& v, float s);