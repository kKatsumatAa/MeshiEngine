#include "Util.h"
#include "WindowsApp.h"

void Vec4xM4(Vec4& v, const M4& m4)
{
	float v4[2][4] = {
		{ v.x_,v.y_,v.z_,v.w_ },
		{0,0,0,0}
	};

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m_[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] ,v4[1][3] };
}

void Vec3xM4(Vec3& v, const M4& m4, bool w)
{
	double v4[2][4] = {
		{ v.x_,v.y_,v.z_,(double)w },
		{0,0,0,0}
	};

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * m4.m_[j][i];
		}
	}

	v = { (float)v4[1][0],(float)v4[1][1] ,(float)v4[1][2] };
}

Vec3 GetVec3xM4(Vec3 v, const M4& m4, bool w)
{
	float v4[2][4] = {
	{ v.x_,v.y_,v.z_,(float)w },
	{0,0,0,0}
	};

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m_[j][i];
		}
	}

	return { v4[1][0],v4[1][1] ,v4[1][2] };
}

void Vec3xM4andDivisionW(Vec3& v, const M4& m4, bool w)
{
	float v4[2][4] = {
		{ v.x_,v.y_,v.z_,(float)w },
		{0,0,0,0}
	};

	for (int32_t i = 0; i < 4; i++)
	{
		for (int32_t j = 0; j < 4; j++)
		{
			v4[1][i] += v4[0][j] * (float)m4.m_[j][i];
		}
	}

	v = { v4[1][0],v4[1][1] ,v4[1][2] };

	if (w == true && v4[1][3] != 0)
	{
		v /= v4[1][3];
	}
	else if (v.z_ != 0)
	{
		float W = v.z_;

		v /= W;
	}
}


//---------------------------------------------

float AngletoRadi(float angle)
{
	return angle * PI / 180;
}

float RaditoAngle(float radian)
{
	return radian / PI * 180;
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


Vec3 SplinePosition(const std::vector<Vec3>& points, int32_t startIndex, float t)
{
	//補完すべき点の数
	size_t n = points.size() - 2;

	if (startIndex > n)return points[n];
	if (startIndex < 1)return points[1];

	//p0~p3の制御点を取得　※p1~p2を補間
	Vec3 p0 = points[startIndex - 1];
	Vec3 p1 = points[startIndex];
	Vec3 p2 = points[startIndex + 1];
	Vec3 p3 = points[startIndex + 2];

	//catmull-rom
	Vec3 position = (
		2.0f * p1 + (-1.0f * p0 + p2) * t
		+ (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * powf(t, 2)
		+ (-1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * powf(t, 3)
		) / 2.0f;

	return position;
}

float EaseIn(float t)
{
	return 1 - cos((t * 3.14f) / 2.0f);
}

float EaseOut(float t)
{
	return sin((t * 3.14f) / 2.0f);
}

bool CollisionCircleCircle(const Vec3& pos1, float r1, const Vec3& pos2, float r2)
{
	if (pow(pos2.x_ - pos1.x_, 2) + pow(pos2.y_ - pos1.y_, 2) + pow(pos2.z_ - pos1.z_, 2)
		<= pow(r1 + r2, 2))
	{
		return true;
	}

	return false;
}

bool CollisionRayCircle(const Vec3& sv, const Vec3& ev, float r, const Vec3& pos, float r2)
{
	//レイとの当たり判定
	Vec3 rayLength = ev - sv;
	/*Vec3 ev = { worldTransform2_.translation_.x + rayLength.x,
		worldTransform2_.translation_.y + rayLength.y,
		worldTransform2_.translation_.z + rayLength.z / 2 };
	Vec3 sv = { worldTransform2_.translation_.x,
		worldTransform2_.translation_.y - rayLength.y,
		worldTransform2_.translation_.z - rayLength.z / 2 };*/
	rayLength.Normalized();
	Vec3 objLength = pos - sv;
	Vec3 dotPos = sv + rayLength * rayLength.Dot(objLength);
	Vec3 dotVec = pos - dotPos;
	float dotLength = dotVec.GetLength();
	if (dotLength <= r + r2
		/*&& (worldTransform_.translation_.z + worldTransform_.scale_.z > sv.z
			&& worldTransform_.translation_.z - worldTransform_.scale_.z < ev.z)*/)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CollisionBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t r1, int32_t r2)
{
	return x1 - r1 < x2 + r2 && x2 - r2 < x1 + r1 &&
		y1 - r1 < y2 + r2 && y2 - r2 < y1 + r1;

	return false;
}

Vec2 Vec3toVec2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection)
{
	//view,projection,viewport行列を掛ける
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftX
		,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftY,0,1
	};

	XMMATRIX mVPVp = view * projection * viewPort;


	Vec3 vec3 = v;

	/*M4 m4 = {
		(float)mVPVp.r[0].m128_f32[0],(float)mVPVp.r[0].m128_f32[1],(float)mVPVp.r[0].m128_f32[2],(float)mVPVp.r[0].m128_f32[3],
		(float)mVPVp.r[1].m128_f32[0],(float)mVPVp.r[1].m128_f32[1],(float)mVPVp.r[1].m128_f32[2],(float)mVPVp.r[1].m128_f32[3],
		(float)mVPVp.r[2].m128_f32[0],(float)mVPVp.r[2].m128_f32[1],(float)mVPVp.r[2].m128_f32[2],(float)mVPVp.r[2].m128_f32[3],
		(float)mVPVp.r[3].m128_f32[0],(float)mVPVp.r[3].m128_f32[1],(float)mVPVp.r[3].m128_f32[2],(float)mVPVp.r[3].m128_f32[3]
	};*/
	M4 m4;
	m4.PutinXMMATRIX(mVPVp);

	Vec3xM4andDivisionW(vec3, m4, 1);

	return Vec2(vec3.x_, vec3.y_);
}

Vec3 Vec2toVec3(const Vec2& v, const XMMATRIX& view, const XMMATRIX& projection, float distance)
{
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftX
		,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftY,0,1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;
	//m4.PutinXMMATRIX(mVPVp);

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);
	//m4.SetInverseMatrix();

	m4.PutinXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { v.x_,v.y_,0 };
	Vec3 posFar = { v.x_,v.y_,1 };

	//スクリーン座標->ワールド座標
	Vec3xM4andDivisionW(posNear, m4, 1);
	Vec3xM4andDivisionW(posFar, m4, 1);

	//nearからfarへの線分
	Vec3 nearFarDirection = posFar - posNear;
	nearFarDirection.Normalized();

	//カメラから照準オブジェクトの距離
	const float DISTANCE_TEST_OBJECT = distance;//仮

	Vec3 pos = posNear + nearFarDirection * DISTANCE_TEST_OBJECT;

	return Vec3(pos);
}

void Vec2toNearFarPos(const Vec2& pos, Vec3& returnNearPos, Vec3& returnFarPos, const XMMATRIX& view, const XMMATRIX& projection)
{
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftX
		,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().viewport_.TopLeftY,0,1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;
	//m4.PutinXMMATRIX(mVPVp);

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);
	//m4.SetInverseMatrix();

	m4.PutinXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { pos.x_,pos.y_,0 };
	Vec3 posFar = { pos.x_,pos.y_,1 };

	//スクリーン座標->ワールド座標
	Vec3xM4andDivisionW(posNear, m4, 1);
	Vec3xM4andDivisionW(posFar, m4, 1);

	returnNearPos = posNear;
	returnFarPos = posFar;
}

float sign(float num)
{
	if (num < 0) {
		return (-1.0f);
	}
	if (num > 0) {
		return (1.0f);
	}
	return 0.0f;
}

int32_t AligmentSize(int32_t size, int32_t aligment)
{
	return size + aligment - size % aligment;
}

void ConstCharToWcharT(const char* string, wchar_t(&wString)[128])
{
	size_t size = _countof(wString);
	mbstowcs_s(&size, wString, string, size);
}

void ConstWCharTToChar(const wchar_t* wString, char(&string)[128])
{
	size_t size = _countof(string);
	wcstombs_s(&size, string, wString, size);
}

float SmoothStep(float Min, float Max, float V)
{
	V = (V - Min) / (Max - Min);
	float d = V - 1.0f;
	long  r = (*(long*)&V ^ 0x3F800000) & (*(long*)&d >> 31);
	r ^= 0x3F800000;
	r &= ~(r >> 31);
	float x = *(float*)&r;
	return (x * x * (3.0f - (x + x)));
}

Vec3 GetRotFromQuaternion(Quaternion q)
{
	Vec3 retRotVec = {};

	//Quaternion r = transform.rotation;
	float x = q.x_;
	float y = q.y_;
	float z = q.z_;
	float w = q.w_;

	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;

	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	// 1 - 2y^2 - 2z^2
	float m00 = 1.0f - (2.0f * y2) - (2.0f * z2);

	// 2xy + 2wz
	float m01 = (2.0f * xy) + (2.0f * wz);

	// 2xy - 2wz
	float m10 = (2.0f * xy) - (2.0f * wz);

	// 1 - 2x^2 - 2z^2
	float m11 = 1.0f - (2.0f * x2) - (2.0f * z2);

	// 2xz + 2wy
	float m20 = (2.0f * xz) + (2.0f * wy);

	// 2yz+2wx
	float m21 = (2.0f * yz) - (2.0f * wx);

	// 1 - 2x^2 - 2y^2
	float m22 = 1.0f - (2.0f * x2) - (2.0f * y2);


	float tx, ty, tz;

	if (Approximately(m21, 1.0f))
	{
		tx = PI / 2.0f;
		ty = 0;
		tz = atan2(m10, m00);
	}
	else if (Approximately(m21, -1.0f))
	{
		tx = -PI / 2.0f;
		ty = 0;
		tz = atan2(m10, m00);
	}
	else
	{
		tx = asin(-m21);
		ty = atan2(m20, m22);
		tz = atan2(m01, m11);
	}

	//tx *= Rad2Deg;
	//ty *= Rad2Deg;
	//tz *= Rad2Deg;

	return Vec3(tx, ty, tz);
}

Vec3 GetRotFromMat(M4 m)
{
	double threshold = 0.001;

	Vec3 ansRot = {};

	if (abs(m.m_[2][1] - 1.0) < threshold) { // R(2,1) = sin(x) = 1の時
		ansRot.x_ = PI / 2;
		ansRot.y_ = 0;
		ansRot.z_ = (float)atan2(m.m_[1][0], m.m_[0][0]);
	}
	else if (abs(m.m_[2][1] + 1.0) < threshold) { // R(2,1) = sin(x) = -1の時
		ansRot.x_ = -PI / 2;
		ansRot.y_ = 0;
		ansRot.z_ = (float)atan2(m.m_[1][0], m.m_[0][0]);
	}
	else
	{
		ansRot.x_ = (float)asin(m.m_[2][1]);
		ansRot.y_ = (float)atan2(-m.m_[2][0], m.m_[2][2]);
		ansRot.z_ = (float)atan2(-m.m_[0][1], m.m_[1][1]);
	}
	return ansRot;
}

bool Approximately(float a, float b)
{
	return (fabs(a - b) <= 0.1f);
}

Vec3 GetTurnVec3UseQuaternionAndRot(const Vec3& vec, const Vec3& rot)
{
	Quaternion qZ = Quaternion::MakeAxisAngle({ 0,0,1.0f }, rot.z_);
	Quaternion qX = Quaternion::MakeAxisAngle({ 1.0f,0,0 }, rot.x_);
	Quaternion qY = Quaternion::MakeAxisAngle({ 0,1.0f,0 }, rot.y_);

	Vec3 ansFrontV = vec;
	ansFrontV = qZ.GetRotateVector(ansFrontV);
	ansFrontV = qX.GetRotateVector(ansFrontV);
	ansFrontV = qY.GetRotateVector(ansFrontV);

	return ansFrontV;
}
