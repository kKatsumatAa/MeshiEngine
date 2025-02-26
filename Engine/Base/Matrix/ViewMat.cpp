#include "ViewMat.h"

using namespace DirectX;


ViewMat::ViewMat(float eyeX, float eyeY, float eyeZ,
	float targetX, float targetY, float targetZ,
	float upX, float upY, float upZ):
	eye_({ eyeX,eyeY,eyeZ }), target_({ targetX,targetY,targetZ }), up_({ upX,upY,upZ })
{
	CalcMat();
}

ViewMat::ViewMat():
	eye_(EYE_), target_(TARGET_), up_(UP_)
{
	CalcMat();
}

void ViewMat::Update()
{
	CalcMat();
}

void ViewMat::CalcMat()
{
	XMFLOAT3 eye2 = { eye_.x,eye_.y ,eye_.z };
	XMFLOAT3 target2 = { target_.x,target_.y ,target_.z };
	XMFLOAT3 up2 = { up_.x,up_.y ,up_.z };
	matView_ = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye2), XMLoadFloat3(&target2), XMLoadFloat3(&up2));
}
