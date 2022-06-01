#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class ViewMat
{
private:

public:
	float angle = 0.0f;//ƒJƒƒ‰‚Ì‰ñ“]Šp

	XMMATRIX matView;
	XMFLOAT3 eye;
	XMFLOAT3 target;
	XMFLOAT3 up;
	

	//
	ViewMat(float eyeX, float eyeY, float eyeZ,
		float targetX, float targetY, float targetZ,
		float upX, float upY, float upZ);
	ViewMat();
	void Update();
	void SetMat();
};