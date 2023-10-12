#include "CollisionPrimitive.h"
using namespace DirectX;



void Triangle::ComputeNormal()
{
	XMVECTOR p0_p1 = iP1 - iP0;
	XMVECTOR p0_p2 = iP2 - iP0;

	//外積により、2辺に垂直なベクトルを算出
	iNormal = XMVector3Cross(p0_p1, p0_p2);
	iNormal = XMVector3Normalize(iNormal);
}
