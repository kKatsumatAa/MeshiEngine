#pragma once
#include <DirectXMath.h>
#include "WindowsApp.h"
using namespace DirectX;

class ProjectionMat
{
private:

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