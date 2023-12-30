/*
* @file Quaternion.h
* @brief クォータニオン
*/

#pragma once
#include"M4.h"

//クォータニオン
class Quaternion
{
private:
	static const float MAKE_AXIS_ANGLE_RATE_;
	static const float MAKE_ROTATE_MAT_RATE_;
public:
	static const float SLERP_RATIO_MAX_;

public://変数
	//v
	float x;
	float y;
	float z;
	//w
	float w;


public://関数
	//コンストラクタ
	Quaternion()
		: x(0), y(0), z(0), w(0) {};
	//コンストラクタ（引数アリ）
	Quaternion(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {};

public:
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
	//プラス
	Quaternion  operator+();
	//クォータニオン同士を足して代入
	Quaternion& operator+=(const Quaternion& other);
	//マイナス
	Quaternion  operator-()const;
	//クォータニオンで引いて代入
	Quaternion& operator-=(const Quaternion& other);
	//floatと掛ける
	Quaternion  operator*(float other);
	//floatと掛けて代入
	Quaternion& operator*=(float other);
	//floatで割る
	Quaternion  operator/(float other);
	//floatで割って代入
	Quaternion& operator/=(float other);

	//ほかのクォータニオンとの掛け算
	Quaternion  operator*(const Quaternion& other);
	//ほかのクォータニオンとの掛けて代入
	Quaternion& operator*=(const Quaternion& other);
};

//二項演算子オーバーロード
//クォータニオン同士の足し算
const Quaternion operator +(const Quaternion& q1, const Quaternion& q2);
//クォータニオン同士の引き算
const Quaternion operator -(const Quaternion& q1, const Quaternion& q2);
//クォータニオンとfloatの掛け算
const Quaternion operator *(const Quaternion& v, float s);
//クォータニオンとfloatの掛け算
const Quaternion operator *(float s, const Quaternion& v);
//クォータニオンをfloatで割る
const Quaternion operator /(const Quaternion& v, float s);
//クォータニオン同士の掛け算
Quaternion  operator*(const Quaternion& q1, const Quaternion& q2);

//球面線形補完
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
//球面線形補完(Vec3を使って)
Quaternion SlerpUseVec3(const Vec3& v0, const Vec3& v1, float t);