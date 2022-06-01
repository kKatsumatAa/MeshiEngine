#include "ViewMat.h"

ViewMat::ViewMat(float eyeX, float eyeY, float eyeZ,
	float targetX, float targetY, float targetZ,
	float upX, float upY, float upZ):
	eye({ eyeX,eyeY,eyeZ }), target({ targetX,targetY,targetZ }), up({ upX,upY,upZ })
{
	SetMat();
}

ViewMat::ViewMat():
	eye({ 0,0,-10.0f }), target({ 0,0,0 }), up({ 0,1.f,0 })
{
	SetMat();
}

void ViewMat::Update()
{
	SetMat();
}

void ViewMat::SetMat()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}
