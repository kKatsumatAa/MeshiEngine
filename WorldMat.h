#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class WorldMat
{
private:
	void SetScale();
	void SetRot();
	void SetTrans();
	XMMATRIX matScale;
	XMMATRIX matRot;
	XMMATRIX matTrans;

public:
	XMMATRIX matWorld;
	XMFLOAT3 scale = { 0,0,0 };
	XMFLOAT3 rot = { 0,0,0 };
	XMFLOAT3 trans = { 0,0,0 };


	//
	WorldMat();
	void SetWorld();
};