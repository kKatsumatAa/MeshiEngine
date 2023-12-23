/*
* @file Vec4.h
* @brief 四次元ベクトル
*/

#pragma once
#include<DirectXMath.h>

class Vec4
{
private:


public:
	float x;
	float y;
	float z;
	float w;

	Vec4()
		:x(0), y(0), z(0), w(0)
	{}
	Vec4(float x, float y, float z, float w)
		:x(x), y(y), z(z), w(w)
	{}

	//便利関数
	float GetLength() const;
	void Normalized();
	Vec4 GetNormalized() const;
	Vec4 Cross(const Vec4& other) const;
	float Dot(const Vec4& other) const;

	//オバロ演算子
	//プラス
	Vec4 operator+()const;
	//ほかのVec4と足して代入
	Vec4& operator+=(const Vec4& other);
	//マイナス
	Vec4 operator-()const;
	//ほかのVec4で引いて代入
	Vec4& operator-=(const Vec4& other);
	//floatと掛ける
	Vec4 operator*(float other);
	//floatと掛けて代入
	Vec4& operator*=(float other);
	//floatで割る
	Vec4 operator/(float other);
	//floatで割って代入
	Vec4& operator/=(float other);
};

//二項演算子オーバーロード
//ほかのVec4と足す
const Vec4 operator +(const Vec4& v1, const Vec4& v2);
//ほかのVec4で引く
const Vec4 operator -(const Vec4& v1, const Vec4& v2);
//floatと掛ける
const Vec4 operator *(const Vec4& v, float s);
//floatと掛ける
const Vec4 operator *(float s, const Vec4& v);
//floatで割る
const Vec4 operator /(const Vec4& v, float s);