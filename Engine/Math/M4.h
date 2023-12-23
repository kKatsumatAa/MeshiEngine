/*
* @file M4.h
* @brief 4x4の行列
*/

#pragma once
#include"Vec3.h"


class M4
{
private://エイリアス
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//行列
	double m_[4][4];

	static const float NORMAL_M[4][4];

public:
	//逆行列計算
	static void CalcInvMat(M4& mat);

public:
	//コンストラクタ
	M4();
	//コンストラクタ（引数アリ）
	M4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	//コンストラクタ(引数が配列)
	M4(const float(*other)[4]);

public:
	//行列を転置
	void TransposeM4();

	//スケール行列セット
	void SetScaleMatrix(const Vec3& v3);
	//回転行列セット
	void SetRotationMatrix(float angle, char axis);
	//位置行列セット
	void SetTranslationMatrix(const Vec3& v3);

	//逆行列にする
	void SetInverseMatrix();
	//逆行列を取得
	M4 GetInverseMatrix() const;

public:
	//このマトリックスクラスにXMMATRIXの中身を入れる
	void PutInXMMATRIX(const XMMATRIX& xM);
	//M4の中身をXMMATRIXに入れる
	void MatIntoXMMATRIX(XMMATRIX& xM)const;

public:
	//行列同士の掛け算
	M4 operator*(const M4& other)const;
	//行列同士をかけて代入
	M4& operator*=(const M4& other);
};
//Vec3と行列の掛け算
const Vec3 operator*(const Vec3& v, const M4& m2);

