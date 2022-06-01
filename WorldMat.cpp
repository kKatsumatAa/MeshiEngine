#include "WorldMat.h"

WorldMat::WorldMat()
{
}

void WorldMat::SetScale(float x, float y, float z)
{
	matScale = XMMatrixScaling(x, y, z);
	matWorld *= matScale;
}

void WorldMat::SetRot(float angleX, float angleY, float angleZ)
{
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(angleZ));
	matRot *= XMMatrixRotationX(XMConvertToRadians(angleX));
	matRot *= XMMatrixRotationY(XMConvertToRadians(angleY));
	matWorld *= matRot;
}

void WorldMat::SetTrans(float x, float y, float z)
{
	matTrans = XMMatrixTranslation(x, y, z);
	matWorld *= matTrans;
}
