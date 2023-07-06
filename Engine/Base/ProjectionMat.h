#pragma once
#include <DirectXMath.h>
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
	XMMATRIX matProjection_;
	float nearPos_;
	float farPos_;
	XMFLOAT2 aspect_;
	float fovY_;

	//
	ProjectionMat();
	ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY,float fovY);
	void SetMat();
};