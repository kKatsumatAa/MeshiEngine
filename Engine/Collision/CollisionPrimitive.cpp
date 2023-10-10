#include "CollisionPrimitive.h"
using namespace DirectX;



void Triangle::ComputeNormal()
{
	XMVECTOR p0_p1 = iP1 - iP0;
	XMVECTOR p0_p2 = iP2 - iP0;

	//螟也ｩ阪↓繧医ｊ縲・霎ｺ縺ｫ蝙ら峩縺ｪ繝吶け繝医Ν繧堤ｮ怜・
	iNormal = XMVector3Cross(p0_p1, p0_p2);
	iNormal = XMVector3Normalize(iNormal);
}
