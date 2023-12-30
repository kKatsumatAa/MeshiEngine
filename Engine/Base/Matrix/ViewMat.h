/*
* @file ViewMat.h
* @brief ビュー行列
*/

#pragma once
#include "Vec3.h"

class ViewMat
{
private://エイリアス
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	const Vec3 EYE_ = { 0,0,-100.0f };
	const Vec3 TARGET_ = { 0,0,0 };
	const Vec3 UP_ = { 0,1.0f,0 };

public:
	XMMATRIX matView_;
	Vec3 eye_;
	Vec3 target_;
	Vec3 up_;

public:
	//コンストラクタ(引数アリ)
	ViewMat(float eyeX, float eyeY, float eyeZ,
		float targetX, float targetY, float targetZ,
		float upX, float upY, float upZ);
	//コンストラクタ
	ViewMat();

	//更新
	void Update();
	//行列計算
	void CalcMat();
};