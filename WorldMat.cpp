#include "WorldMat.h"
#include "Util.h"

WorldMat::WorldMat()
{
	matWorld = 
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matScale =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matRot =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matTrans =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };
}

void WorldMat::SetScale()
{
	matScale = XMMatrixScaling( scale.x, scale.y, scale.z);
	matWorld *= matScale;
}

void WorldMat::SetRot()
{
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));
	matWorld *= matRot;
}

void WorldMat::SetTrans()
{
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	matWorld *= matTrans;
}

void WorldMat::SetWorld()
{
	matWorld =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	SetScale();
	SetRot();
	SetTrans();
}
