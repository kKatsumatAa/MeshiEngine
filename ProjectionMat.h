#pragma once
#include <DirectXMath.h>
#include "WindowsApp.h"
using namespace DirectX;

class ProjectionMat
{
private:

public:
	XMMATRIX matProjection;
	float nearPos;
	float farPos;
	XMFLOAT2 aspect;
	float fovY;

	//
	ProjectionMat(WindowsApp& win);
	ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY,float fovY);
	void SetMat();
};