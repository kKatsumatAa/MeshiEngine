#include "Util.h"
#include "WindowsApp.h"

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
		{ v.x,v.y,v.z,w },
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

void Vec3xM4andDivisionW(Vec3& v, const M4& m4, const bool w)
{
	float v4[2][4] = {
		{ v.x,v.y,v.z,w },
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

	float W = v.z;

	v /= W;
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

float GetRadianVec3(const Vec3& v1, const Vec3& v2)
{
	return acos(v1.Dot(v2) / (v1.GetLength() * v2.GetLength()));
}

//----------------------------------------------------------------------
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t)
{
	return v1 + t * (v2 - v1);
}

Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t)
{
	float radian = GetRadianVec3(v1, v2);

	Vec3 v = ((1 - t) * v1.GetLength() + t * v2.GetLength()) *
		(sinf((1 - t) * radian) / sinf(radian) * v1.GetNormalized() + sinf(t * radian) / sinf(radian) * v2.GetNormalized());

	return v;
}

bool CollisionCircleCircle(const Vec3& pos1, const float& r1, const Vec3& pos2, const float& r2)
{
	if (pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2)
		<= pow(r1 + r2, 2))
	{
		return true;
	}

	return false;
}

XMFLOAT2 Vec3toXMFLOAT2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection)
{
	//view,projection,viewports—ñ‚ðŠ|‚¯‚é
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().window_width / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().window_height / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().window_width / 2.0f + WindowsApp::GetInstance().viewport.TopLeftX
		,WindowsApp::GetInstance().window_height / 2.0f + WindowsApp::GetInstance().viewport.TopLeftY,0,1
	};

	XMMATRIX mVPVp = view * projection * viewPort;


	Vec3 vec3 = v;

	M4 m4 = { 
		(float)mVPVp.r[0].m128_f32[0],(float)mVPVp.r[0].m128_f32[1],(float)mVPVp.r[0].m128_f32[2],(float)mVPVp.r[0].m128_f32[3],
		(float)mVPVp.r[1].m128_f32[0],(float)mVPVp.r[1].m128_f32[1],(float)mVPVp.r[1].m128_f32[2],(float)mVPVp.r[1].m128_f32[3],
		(float)mVPVp.r[2].m128_f32[0],(float)mVPVp.r[2].m128_f32[1],(float)mVPVp.r[2].m128_f32[2],(float)mVPVp.r[2].m128_f32[3],
		(float)mVPVp.r[3].m128_f32[0],(float)mVPVp.r[3].m128_f32[1],(float)mVPVp.r[3].m128_f32[2],(float)mVPVp.r[3].m128_f32[3] 
	};

	Vec3xM4andDivisionW(vec3, m4, 0);

	return XMFLOAT2(vec3.x, vec3.y);
}
