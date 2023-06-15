#pragma once
#include <DirectXMath.h>
#include "Vec3.h"
using namespace DirectX;

class ViewMat
{
private:

public:
	XMMATRIX matView_;
	Vec3 eye_;
	Vec3 target_;
	Vec3 up_;
	

	//
	ViewMat(float eyeX, float eyeY, float eyeZ,
		float targetX, float targetY, float targetZ,
		float upX, float upY, float upZ);
	ViewMat();
	void Update();
	void SetMat();
};