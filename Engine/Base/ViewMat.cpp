#include "ViewMat.h"

using namespace DirectX;


ViewMat::ViewMat(float eyeX, float eyeY, float eyeZ,
	float targetX, float targetY, float targetZ,
	float upX, float upY, float upZ):
	eye_({ eyeX,eyeY,eyeZ }), target_({ targetX,targetY,targetZ }), up_({ upX,upY,upZ })
{
	SetMat();
}

ViewMat::ViewMat():
	eye_({ 0,0,-100.0f }), target_({ 0,0,0 }), up_({ 0,1.f,0 })
{
	SetMat();
}

void ViewMat::Update()
{
	SetMat();
}

void ViewMat::SetMat()
{
	XMFLOAT3 eye2 = { eye_.x_,eye_.y_ ,eye_.z_ };
	XMFLOAT3 target2 = { target_.x_,target_.y_ ,target_.z_ };
	XMFLOAT3 up2 = { up_.x_,up_.y_ ,up_.z_ };
	matView_ = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye2), XMLoadFloat3(&target2), XMLoadFloat3(&up2));
}
