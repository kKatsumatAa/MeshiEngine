#include "M4.h"

using namespace DirectX;

const float M4::NORMAL_M[4][4] = {
	   {1,0,0,0},
	   {0,1,0,0},
	   {0,0,1,0},
	   {0,0,0,1}
};

void M4::CalcInvMat(M4& mat)
{
	double inv[16], det;
	double m[16];

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			m[i * 4 + j] = mat.m_[i][j];
		}
	}

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return;

	det = 1.0 / det;

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			mat.m_[i][j] = inv[i * 4 + j] * det;
		}
	}
}

M4::M4()
{
	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			m_[i][j] = 0;
		}
	}
}

M4::M4(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m_[0][0] = m00;
	m_[0][1] = m01;
	m_[0][2] = m02;
	m_[0][3] = m03;
	m_[1][0] = m10;
	m_[1][1] = m11;
	m_[1][2] = m12;
	m_[1][3] = m13;
	m_[2][0] = m20;
	m_[2][1] = m21;
	m_[2][2] = m22;
	m_[2][3] = m23;
	m_[3][0] = m30;
	m_[3][1] = m31;
	m_[3][2] = m32;
	m_[3][3] = m33;
}

M4::M4(const float(*other)[4])
{
	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			m_[i][j] = other[i][j];
		}
	}
}


//-----------------------------------------------------------

void M4::TransposeM4()
{
	M4 M = *this;

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			m_[i][j] = M.m_[j][i];
		}
	}
}


//------------------------------------------------------

void M4::SetScaleMatrix(const Vec3& v3)
{
	*this = {
		v3.x_,0,0,0,
		0,v3.y_,0,0,
		0,0,v3.z_,0,
		0,0,0,1
	};
}

void M4::SetRotationMatrix(float angle, char axis)
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
		v3.x_,v3.y_,v3.z_,1
	};
}

void M4::SetInverseMatrix()
{
	CalcInvMat(*this);
}

M4 M4::GetInverseMatrix()const
{
	M4 mat = *this;

	CalcInvMat(mat);

	return M4(mat);
}

//M4& M4::operator=(const float (*other)[4])
//{
//	for (int32_t i = 0; i < 4; i++)
//	{
//		for (int32_t j = 0; j < 4; j++)
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

	for (int32_t ic = 0; ic < 4; ic++)
	{
		for (int32_t i = 0; i < 4; i++)
		{
			for (int32_t j = 0; j < 4; j++)
			{
				m4.m_[ic][i] += m_[ic][j] * other.m_[j][i];
			}
		}
	}

	return m4;
}

M4& M4::operator*=(const M4& other)
{
	M4 m4;

	for (int32_t ic = 0; ic < 4; ic++)
	{
		for (int32_t i = 0; i < 4; i++)
		{
			for (int32_t j = 0; j < 4; j++)
			{
				m4.m_[ic][i] += m_[ic][j] * other.m_[j][i];
			}
		}
	}
	*this = m4;

	return *this;
}

void M4::PutInXMMATRIX(const XMMATRIX& xM)
{
	*this = {
		(float)xM.r[0].m128_f32[0],(float)xM.r[0].m128_f32[1],(float)xM.r[0].m128_f32[2],(float)xM.r[0].m128_f32[3],
		(float)xM.r[1].m128_f32[0],(float)xM.r[1].m128_f32[1],(float)xM.r[1].m128_f32[2],(float)xM.r[1].m128_f32[3],
		(float)xM.r[2].m128_f32[0],(float)xM.r[2].m128_f32[1],(float)xM.r[2].m128_f32[2],(float)xM.r[2].m128_f32[3],
		(float)xM.r[3].m128_f32[0],(float)xM.r[3].m128_f32[1],(float)xM.r[3].m128_f32[2],(float)xM.r[3].m128_f32[3]
	};
}

void M4::MatIntoXMMATRIX(XMMATRIX& xM)const
{
	xM = { (float)m_[0][0],(float)m_[0][1],(float)m_[0][2],(float)m_[0][3],
				 (float)m_[1][0],(float)m_[1][1],(float)m_[1][2],(float)m_[1][3],
				 (float)m_[2][0],(float)m_[2][1],(float)m_[2][2],(float)m_[2][3],
				 (float)m_[3][0],(float)m_[3][1],(float)m_[3][2],(float)m_[3][3] };

}

//void M4::Normalize()
//{
//}

const Vec3 operator*(const Vec3& v, const M4& m2)
{
	float w = (float)(v.x_ * m2.m_[0][3] + v.y_ * m2.m_[1][3] + v.z_ * m2.m_[2][3] + m2.m_[3][3]);

	Vec3 result
	{
		(float)(v.x_ * m2.m_[0][0] + v.y_ * m2.m_[1][0] + v.z_ * m2.m_[2][0] + m2.m_[3][0]) / w,
		(float)(v.x_ * m2.m_[0][1] + v.y_ * m2.m_[1][1] + v.z_ * m2.m_[2][1] + m2.m_[3][1]) / w,
		(float)(v.x_ * m2.m_[0][2] + v.y_ * m2.m_[1][2] + v.z_ * m2.m_[2][2] + m2.m_[3][2]) / w
	};

	return result;
}
