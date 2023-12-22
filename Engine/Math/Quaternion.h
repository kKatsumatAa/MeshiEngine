/*
* @file Quaternion.h
* @brief クォータニオン
*/

#pragma once
#include"M4.h"

//クォータニオン
class Quaternion
{
public://変数
	//v
	float x;
	float y;
	float z;
	//w
	float w;


public://関数
	Quaternion()
		: x(0), y(0), z(0), w(0) {};
	Quaternion(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {};


	//積
	Quaternion GetMultiply(const Quaternion& rhs) const;
	//共役クォータニオンを返す
	Quaternion GetConjugate() const;
	//クォータニオンのNormを返す
	float GetLength() const;
	//正規化したクォータニオンを返す
	Quaternion GetNormalize() const;
	//逆クォータニオンを返す
	Quaternion GetInverse() const;


	//ベクトルをクォータニオンで回転させた結果のベクトルを求める
	Vec3 GetRotateVector(const Vec3& vector) const;
	//クォータニオンから回転行列を求める
	M4 MakeRotateMatrix() const;

	//内積
	float DotQuaternion(const Quaternion& rhs)const;

	//x,y,zをセット
	void SetVec(const Vec3& vec);


public://静的メンバ関数
	//単位クォータニオンを生成
	static Quaternion GetIdentityQuaternion();

	//任意軸回転を表すクォータニオンを返す
	static Quaternion MakeAxisAngle(const Vec3& axis, float angle, const Vec3& useAxis = { 1.0f,1.0f,1.0f });

	//uからvへの回転を生成
	static Quaternion DirectionToDirection(const Vec3& u, const Vec3& v, const Vec3& useAxis = { 1.0f,1.0f,1.0f });


public://オバロ演算子
	Quaternion  operator+();
	Quaternion& operator+=(const Quaternion& other);
	Quaternion  operator-()const;
	Quaternion& operator-=(const Quaternion& other);
	Quaternion  operator*(float other);
	Quaternion& operator*=(float other);
	Quaternion  operator/(float other);
	Quaternion& operator/=(float other);

	Quaternion  operator*(const Quaternion& other);
	Quaternion& operator*=(const Quaternion& other);
};



//二項演算子オーバーロード
const Quaternion operator +(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator -(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator *(const Quaternion& v, float s);
const Quaternion operator *(float s, const Quaternion& v);
const Quaternion operator /(const Quaternion& v, float s);

Quaternion  operator*(const Quaternion& q1, const Quaternion& q2);



//球面線形補完
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
Quaternion SlerpUseVec3(const Vec3& v0, const Vec3& v1, float t);



