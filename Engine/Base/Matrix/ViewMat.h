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