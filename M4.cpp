#include "M4.h"

M4::M4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = 0;
		}
	}
}

M4::M4(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

M4::M4(const float(*other)[4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = other[i][j];
		}
	}
}


//-----------------------------------------------------------

void M4::TransposeM4()
{
	M4 M = *this;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = M.m[j][i];
		}
	}
}


//------------------------------------------------------

void M4::SetScaleMatrix(const Vec3& v3)
{
	*this = {
		v3.x,0,0,0,
		0,v3.y,0,0,
		0,0,v3.z,0,
		0,0,0,1
	};
}

void M4::SetRotationMatrix(const float& angle, const char& axis)
{
	if (axis == 'x' || axis == 'X')
	{
		*this = {
			1,0,0,0,
			0,cosf(angle),sinf(angle),0,
			0,-sinf(angle),cosf(angle),0,
			0,0,0,1
		};
	}

	if (axis == 'y' || axis == 'Y')
	{
		*this = {
			cosf(angle),0,-sinf(angle),0,
			0,1,0,0,
			sinf(angle),0,cosf(angle),0,
			0,0,0,1
		};
	}

	if (axis == 'z' || axis == 'Z')
	{
		*this = {
			cosf(angle),sinf(angle),0,0,
			-sinf(angle),cosf(angle),0,0,
			0,0,1,0,
			0,0,0,1
		};
	}
}

void M4::SetTranslationMatrix(const Vec3& v3)
{
	*this = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		v3.x,v3.y,v3.z,1
	};
}

//M4& M4::operator=(const float (*other)[4])
//{
//	for (int i = 0; i < 4; i++)
//	{
//		for (int j = 0; j < 4; j++)
//		{
//			m[i][j] = other[i][j];
//		}
//	}
//
//	return *this;
//}

M4 M4::operator*(const M4& other)
{
	M4 m4;

	for (int ic = 0; ic < 4; ic++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m4.m[ic][i] += m[ic][j] * other.m[j][i];
			}
		}
	}

	return m4;
}

M4& M4::operator*=(const M4& other)
{
	M4 m4;

	for (int ic = 0; ic < 4; ic++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m4.m[ic][i] += m[ic][j] * other.m[j][i];
			}
		}
	}
	*this = m4;

	return *this;
}

//void M4::Normalize()
//{
//}
