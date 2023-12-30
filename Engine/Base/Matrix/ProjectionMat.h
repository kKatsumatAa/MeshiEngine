/*
* @file ProjectionMat.h
* @brief プロジェクション行列
*/

#pragma once
#include "WindowsApp.h"

class ProjectionMat
{
private://エイリアス
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	const float NEAR_POS_ = 0.1f;
	const float FAR_POS_ = 10000.0f;
	const float FOV_Y_ANGLE_ = 45.0f;

public:
	XMMATRIX matProjection_;
	float nearPos_;
	float farPos_;
	XMFLOAT2 aspect_;
	float fovY_;

public:
	//コンストラクタ
	ProjectionMat();
	//コンストラクタ(引数アリ)
	ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY, float fovY);
	//行列計算
	void CalcMat();
};