/*
* @file Vec3.h
* @brief 三次元ベクトル
*/

#pragma once
#include"Vec2.h"

//π
const float PI = 3.1415f;
//2π
const float PI_2 = PI * 2.0f;
//180°
const float DEGREE_180 = 180.0f;
//360°
const float DEGREE_360 = DEGREE_180 * 2.0f;

class Vec3
{
public:
	float x;
	float y;
	float z;


public:
	//コンストラクタ
	Vec3()
		:x(0), y(0), z(0)
	{}
	//コンストラクタ（引数アリ）
	Vec3(float x, float y, float z)
		:x(x), y(y), z(z)
	{}
	//コンストラクタ（vec2使用）
	Vec3(const Vec2& xy, float z)
		:x(xy.x), y(xy.y), z(z)
	{}
	//コンストラクタ（vec2使用）
	Vec3(float x, const Vec2& yz)
		:x(x), y(yz.x), z(yz.y)
	{}

public:
	//長さ取得
	float GetLength() const;
	//正規化
	void Normalized();
	//正規化したものを取得
	Vec3 GetNormalized() const;
	//外積取得
	Vec3 Cross(const Vec3& other) const;
	//内積取得
	float Dot(const Vec3& other) const;

	//オバロ演算子
	//プラス
	Vec3 operator+()const;
	//Vec3同士足して代入
	Vec3& operator+=(const Vec3& other);
	//マイナス
	Vec3 operator-()const;
	//Vec3で引いて代入
	Vec3& operator-=(const Vec3& other);
	//floatと掛ける
	Vec3 operator*(float other);
	//floatと掛けて代入
	Vec3& operator*=(float other);
	//floatで割る
	Vec3 operator/(float other);
	//floatで割って代入
	Vec3& operator/=(float other);
};

//二項演算子オーバーロード
//ほかのVec3と足す
const Vec3 operator +(const Vec3& v1, const Vec3& v2);
//ほかのVec3で引く
const Vec3 operator -(const Vec3& v1, const Vec3& v2);
//floatと掛ける
const Vec3 operator *(const Vec3& v, float s);
//floatと掛ける
const Vec3 operator *(float s, const Vec3& v);
//floatで割る
const Vec3 operator /(const Vec3& v, float s);