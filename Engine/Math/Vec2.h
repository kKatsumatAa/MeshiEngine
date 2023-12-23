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
	//コンストラクタ（引数アリ）
	Vec2(float x, float y)
		:x(x), y(y)
	{}

public:
	//長さ取得
	float GetLength() const;
	//正規化する
	void  Normalized();
	//正規化したベクトル取得
	Vec2  GetNormalize() const;
	//内積取得
	float Dot(const Vec2& v) const;
	//外積取得
	float Cross(const Vec2& v) const;

public:
	//マイナス
	Vec2  operator -()const;
	//複合演算子(-)
	Vec2& operator -=(const Vec2& other);
	//プラス
	Vec2  operator +()const;
	//複合演算子(+)
	Vec2& operator +=(const Vec2& other);
	//floatとの掛け算
	Vec2  operator *(float other);
	//複合演算子(*)
	Vec2& operator *=(float other);
	//floatで割る
	Vec2  operator /(float other);
	//複合演算子(/)
	Vec2& operator /=(float other);
};

//二項演算子オーバーロード
//Vec2同士の足し算
const Vec2 operator +(const Vec2& v1, const Vec2& v2);
//Vec2同士の引き算
const Vec2 operator -(const Vec2& v1, const Vec2& v2);
//Vec2をfloatで掛ける
const Vec2 operator *(const Vec2& v, float s);
//Vec2をfloatで掛ける
const Vec2 operator *(float s, const Vec2& v);
//Vec2をfloatで割る
const Vec2 operator /(const Vec2& v, float s);