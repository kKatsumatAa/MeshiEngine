#include "ProjectionMat.h"

using namespace DirectX;


ProjectionMat::ProjectionMat():
	nearPos_(0.1f), farPos_(10000.0f), aspect_({ WindowsApp::GetInstance().WINDOW_WIDTH_ , WindowsApp::GetInstance().WINDOW_HEIGHT_ }),
	fovY_(XMConvertToRadians(45.0f))
{
	SetMat();
}

ProjectionMat::ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY, float fovY):
	nearPos_(nearPos), farPos_(farPos),aspect_({aspectX,aspectY}),fovY_(fovY)
{
	SetMat();
}

void ProjectionMat::SetMat()
{
	matProjection_ = XMMatrixPerspectiveFovLH(
		fovY_,//fovY
		(float)aspect_.x / aspect_.y,//アスペクト比
		nearPos_, farPos_//前端、奥端
	);
}
