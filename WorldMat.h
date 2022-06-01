#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class WorldMat
{
private:


public:
	XMMATRIX matWorld;
	XMMATRIX matScale;
	XMMATRIX matRot;
	XMMATRIX matTrans;


	//
	WorldMat();
	void SetScale(float x, float y, float z);
	void SetRot(float angleX, float angleY, float angleZ);
	void SetTrans(float x, float y, float z);
};