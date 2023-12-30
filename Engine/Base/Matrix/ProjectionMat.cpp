#include "ProjectionMat.h"

using namespace DirectX;


ProjectionMat::ProjectionMat():
	nearPos_(NEAR_POS_), farPos_(FAR_POS_), aspect_({ WindowsApp::GetInstance().WINDOW_WIDTH_ , WindowsApp::GetInstance().WINDOW_HEIGHT_ }),
	fovY_(XMConvertToRadians(FOV_Y_ANGLE_))
{
	CalcMat();
}

ProjectionMat::ProjectionMat(float nearPos, float farPos, float aspectX, float aspectY, float fovY):
	nearPos_(nearPos), farPos_(farPos),aspect_({aspectX,aspectY}),fovY_(fovY)
{
	CalcMat();
}

void ProjectionMat::CalcMat()
{
	matProjection_ = XMMatrixPerspectiveFovLH(
		fovY_,//fovY
		(float)aspect_.x / aspect_.y,//アスペクト比
		nearPos_, farPos_//前端、奥端
	);
}
