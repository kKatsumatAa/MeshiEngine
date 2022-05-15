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

void M4xM4(M4& m4, const M4& other)
{
	M4 m;

	for (int ic = 0; ic < 4; ic++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m.m[ic][i] += m4.m[ic][j] * other.m[j][i];
			}
		}
	}

	m4 = m;
}

void NormVec4(Vec4& v/*, const M4& m4*/)
{
	Vec4xM4(v, normalM);
}

void TransposeM4(M4& m4)
{
	M4 M = m4;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m4.m[i][j] = M.m[j][i];
		}
	}
}

//-----------------------------------------------------------
