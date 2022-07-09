#include "Util.h"

void Vec4xM4(Vec4& v, const M4& m4)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,v.w },
		{0,0,0,0}
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * m4.m[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] ,v4[1][3] };
}

void Vec3xM4(Vec3& v, const M4& m4, const bool w)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,0 },
		{0,0,0,0}
	};

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * m4.m[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] };
}



//---------------------------------------------

float AngletoRadi(const float& angle)
{
	return angle * pi / 180;
}

float RaditoAngle(const float& radian)
{
	return radian / pi * 180;
}
