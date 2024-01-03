#include "Util.h"
#include "WindowsApp.h"
#include <random>

using namespace DirectX;

void Vec4xM4(Vec4& v, const M4& m4)
{
	const int8_t ANS_INDEX = 1;

	float v4[][M4::S_MAT_WIDTH_] = {
		{ v.x,v.y,v.z,v.w },
		{0,0,0,0}
	};

	//掛け算
	for (int32_t i = 0; i < M4::S_MAT_HEIGHT_; i++)
	{
		for (int32_t j = 0; j < M4::S_MAT_WIDTH_; j++)
		{
			v4[ANS_INDEX][i] += v4[0][j] * (float)m4.m_[j][i];
		}
	}

	v = { v4[ANS_INDEX][0],v4[ANS_INDEX][1] ,v4[ANS_INDEX][2] ,v4[ANS_INDEX][3] };
}

void Vec3xM4(Vec3& v, const M4& m4, bool w)
{
	const int8_t ANS_INDEX = 1;

	double v4[][4] = {
		{ v.x,v.y,v.z,(double)w },
		{0,0,0,0}
	};

	//掛け算
	for (int32_t i = 0; i < M4::S_MAT_HEIGHT_; i++)
	{
		for (int32_t j = 0; j < M4::S_MAT_WIDTH_; j++)
		{
			v4[ANS_INDEX][i] += v4[0][j] * m4.m_[j][i];
		}
	}

	v = { (float)v4[ANS_INDEX][0],(float)v4[ANS_INDEX][1] ,(float)v4[ANS_INDEX][2] };
}

Vec3 GetVec3xM4(const Vec3& v, const M4& m4, bool w)
{
	Vec3 ans = v;

	Vec3xM4(ans, m4, w);

	return ans;
}

void Vec3xM4andDivisionW(Vec3& v, const M4& m4, bool w)
{
	const int8_t ANS_INDEX = 1;

	float v4[][4] = {
		{ v.x,v.y,v.z,(float)w },
		{0,0,0,0}
	};

	//掛け算
	for (int32_t i = 0; i < M4::S_MAT_HEIGHT_; i++)
	{
		for (int32_t j = 0; j < M4::S_MAT_WIDTH_; j++)
		{
			v4[ANS_INDEX][i] += v4[0][j] * (float)m4.m_[j][i];
		}
	}

	v = { v4[ANS_INDEX][0],v4[ANS_INDEX][1] ,v4[ANS_INDEX][2] };

	//ある数でベクトルを割る
	if (w == true && v4[ANS_INDEX][3] != 0)
	{
		v /= v4[ANS_INDEX][3];
	}
	else if (v.z != 0)
	{
		float W = v.z;

		v /= W;
	}
}

Vec3 GetVec3xM4andDivisionW(const Vec3& v, const M4& m4, bool w)
{
	Vec3 ans = v;

	Vec3xM4andDivisionW(ans, m4, w);

	return ans;
}


//---------------------------------------------

float AngletoRadi(float angle)
{
	return angle * PI / DEGREE_180;
}

float RaditoAngle(float radian)
{
	return radian / PI * DEGREE_180;
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

float Lerp(float v1, float v2, float t)
{
	return v1 + t * (v2 - v1);
}

Vec2 LerpVec2(const Vec2& v1, const Vec2& v2, float t)
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
	const int8_t N_OFFSET = -2;
	const float WEIGHT_0 = -1.0f;
	const float WEIGHT_1 = 2.0f;
	const float WEIGHT_2 = -5.0f;
	const float WEIGHT_3 = 4.0f;
	const float WEIGHT_4 = 3.0f;

	const int8_t POW_0 = 2;
	const int8_t POW_1 = 3;

	//補完すべき点の数
	size_t n = points.size() + N_OFFSET;

	if (startIndex > n)return points[n];
	if (startIndex < 1)return points[1];

	//p0~p3の制御点を取得　※p1~p2を補間
	Vec3 p0 = points[startIndex - 1];
	Vec3 p1 = points[startIndex];
	Vec3 p2 = points[startIndex + 1];
	Vec3 p3 = points[startIndex + 2];

	const float NORMALIZER = 2.0f;

	//catmull-rom
	Vec3 position = (
		WEIGHT_1 * p1 + (WEIGHT_0 * p0 + p2) * t
		+ (WEIGHT_1 * p0 + WEIGHT_2 * p1 + WEIGHT_3 * p2 - p3) * powf(t, POW_0)
		+ (WEIGHT_0 * p0 + WEIGHT_4 * p1 - WEIGHT_4 * p2 + p3) * powf(t, POW_1)
		) / NORMALIZER;

	return position;
}


//---------------------------------------------------------------------------------
float EaseIn(float t)
{
	return 1 - cos((t * PI) / 2.0f);
}

float EaseOut(float t)
{
	return sin((t * PI) / 2.0f);
}

float EaseInOutBack(float t)
{
	const float C1 = 1.70158f;
	const float C2 = C1 * 1.525f;

	const float RATIO_MAX = 1.0f;

	return t < 0.5f
		? (powf(2.0f * t, 2.0f) * ((C2 + RATIO_MAX) * 2.0f * t - C2)) / 2.0f
		: (powf(2.0f * t - 2.0f, 2.0f) * ((C2 + RATIO_MAX) * (t * 2.0f - 2.0f) + C2) + 2.0f) / 2.0f;
}

float EaseInOut(float t)
{
	return t == 0.0f
		? 0.0f
		: t == 1.0f
		? 1.0f
		: t < 0.5f ? powf(2, 20.0f * t - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20 * t + 10.0f)) / 2.0f;
}


//-----------------------------------------------------------------------------------
bool CollisionCircleCircle(const Vec3& pos1, float r1, const Vec3& pos2, float r2)
{
	if (pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2)
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
	rayLength.Normalized();
	Vec3 objLength = pos - sv;
	Vec3 dotPos = sv + rayLength * rayLength.Dot(objLength);
	Vec3 dotVec = pos - dotPos;
	float dotLength = dotVec.GetLength();
	if (dotLength <= r + r2)
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
}

bool CollisionBox(Vec2 leftUp1, Vec2 rightBottom1, Vec2 leftUp2, Vec2 rightBottom2)
{
	return leftUp1.x < rightBottom2.x && leftUp2.x < rightBottom1.x &&
		leftUp1.y < rightBottom2.y && leftUp2.y < rightBottom1.y;
}

Vec2 Vec3toVec2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection)
{
	//view,projection,viewport行列を掛ける
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftX
		,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftY,0,1
	};

	XMMATRIX mVPVp = view * projection * viewPort;


	Vec3 vec3 = v;

	M4 m4;
	m4.PutInXMMATRIX(mVPVp);

	Vec3xM4andDivisionW(vec3, m4, 1);

	return Vec2(vec3.x, vec3.y);
}

Vec3 Vec2toVec3(const Vec2& v, const XMMATRIX& view, const XMMATRIX& projection, float distance)
{
	XMMATRIX viewPort = {
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,0,0,0,
		0,-WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f,0,0,
		0,0,1,0,

		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftX
		,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftY,0,1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);

	m4.PutInXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { v.x,v.y,0 };
	Vec3 posFar = { v.x,v.y,1 };

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
		//ここから下は一列（長くなるので改行）
		WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftX,
		WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f + WindowsApp::GetInstance().GetViewport()->TopLeftY,
		0,
		1
	};

	//合成行列
	XMMATRIX mVPVp = view * projection * viewPort;
	M4 m4;

	//逆行列計算
	XMMATRIX mInverseVPVp = XMMatrixInverse(nullptr, mVPVp);

	m4.PutInXMMATRIX(mInverseVPVp);

	//スクリーン座標
	Vec3 posNear = { pos.x,pos.y,0 };
	Vec3 posFar = { pos.x,pos.y,1 };

	//スクリーン座標->ワールド座標
	Vec3xM4andDivisionW(posNear, m4, 1);
	Vec3xM4andDivisionW(posFar, m4, 1);

	returnNearPos = posNear;
	returnFarPos = posFar;
}

float sign(float num)
{
	const float RATIO_MAX = 1.0f;

	if (num < 0) {
		return (-RATIO_MAX);
	}
	if (num > 0) {
		return (RATIO_MAX);
	}
	return 0.0f;
}

int32_t AligmentSize(int32_t size, int32_t aligment)
{
	return size + aligment - size % aligment;
}

void ConstCharToWcharT(const char* string, wchar_t(&wString)[Constant::S_TRANS_W_CHAR_SIZE_])
{
	size_t size = _countof(wString);
	mbstowcs_s(&size, wString, string, size);
}

void ConstWCharTToChar(const wchar_t* wString, char(&string)[Constant::S_TRANS_W_CHAR_SIZE_])
{
	size_t size = _countof(string);
	wcstombs_s(&size, string, wString, size);
}

std::string MySubstr(const std::string& str, char findChar)
{
	// フルパスからファイル名を取り出す
	size_t pos1;
	//文字列の位置を取得し、見つかれば
	pos1 = str.rfind(findChar);
	if (pos1 != std::string::npos) {
		//その位置の一つ後ろからその位置からのサイズ(-1)分取得？（余分な文字列を抜いてる）
		return str.substr(pos1 + 1, str.size() - pos1 - 1);
	}

	return str;
}

std::string ExtractFileName(const std::string& path)
{
	//区切り文字が出てくる一番最後の一つ後ろからの文字列を返す（Resources'/'[***.png]みたいな）

	size_t pos1;
	//区切り文字 '\\'が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != std::string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	//区切り文字 '/'が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != std::string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

float SmoothStep(float min, float max, float v)
{
	const int8_t BIT = 32;
	const uint32_t MASK = 0x3F800000;
	const float RATIO_MAX = 1.0f;
	const float COEFFICIENT = 3.0f;

	v = (v - min) / (max - min);
	float d = v - RATIO_MAX;
	long  r = (*(long*)&v ^ MASK) & (*(long*)&d >> (BIT - 1));
	r ^= MASK;
	r &= ~(r >> (BIT - 1));
	float x = *(float*)&r;
	return (x * x * (COEFFICIENT - (x + x)));
}

float Clamp(float v, float min, float max)
{
	if (v > max)
	{
		return max;
	}
	if (v < min)
	{
		return min;
	}

	return v;
}

Vec3 GetRotFromQuaternion(Quaternion q)
{
	Vec3 retRotVec = {};

	//Quaternion r = transform.rotation;
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;

	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	const float RATIO_MAX_ = 1.0f;

	// 1 - 2y^2 - 2z^2
	float m00 = RATIO_MAX_ - (2.0f * y2) - (2.0f * z2);

	// 2xy + 2wz
	float m01 = (2.0f * xy) + (2.0f * wz);

	// 2xy - 2wz
	float m10 = (2.0f * xy) - (2.0f * wz);

	// 1 - 2x^2 - 2z^2
	float m11 = RATIO_MAX_ - (2.0f * x2) - (2.0f * z2);

	// 2xz + 2wy
	float m20 = (2.0f * xz) + (2.0f * wy);

	// 2yz+2wx
	float m21 = (2.0f * yz) - (2.0f * wx);

	// 1 - 2x^2 - 2y^2
	float m22 = RATIO_MAX_ - (2.0f * x2) - (2.0f * y2);


	float tx, ty, tz;

	if (Approximately(m21, RATIO_MAX_))
	{
		tx = PI / 2.0f;
		ty = 0;
		tz = atan2(m10, m00);
	}
	else if (Approximately(m21, -RATIO_MAX_))
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

	return Vec3(tx, ty, tz);
}

Vec3 GetRotFromMat(M4 m)
{
	double threshold = 0.001;
	const double ADD = 1.0;

	Vec3 ansRot = {};

	if (abs(m.m_[2][1] - ADD) < threshold) { // R(2,1) = sin(x) = 1の時
		ansRot.x = PI / 2.0f;
		ansRot.y = 0;
		ansRot.z = (float)atan2(m.m_[1][0], m.m_[0][0]);
	}
	else if (abs(m.m_[2][1] + ADD) < threshold) { // R(2,1) = sin(x) = -1の時
		ansRot.x = -PI / 2.0f;
		ansRot.y = 0;
		ansRot.z = (float)atan2(m.m_[1][0], m.m_[0][0]);
	}
	else
	{
		ansRot.x = (float)asin(m.m_[2][1]);
		ansRot.y = (float)atan2(-m.m_[2][0], m.m_[2][2]);
		ansRot.z = (float)atan2(-m.m_[0][1], m.m_[1][1]);
	}
	return ansRot;
}

bool Approximately(float a, float b)
{
	const float JUDGE_MIN = 0.1f;

	return (fabs(a - b) <= JUDGE_MIN);
}

Vec3 GetTurnVec3UseQuaternionAndRot(const Vec3& vec, const Vec3& rot)
{
	const Vec3 AXIS_Z = { 0,0,1.0f };
	const Vec3 AXIS_X = { 1.0f,0,0 };
	const Vec3 AXIS_Y = { 0,1.0f,0 };

	Quaternion qZ = Quaternion::MakeAxisAngle(AXIS_Z, rot.z);
	Quaternion qX = Quaternion::MakeAxisAngle(AXIS_X, rot.x);
	Quaternion qY = Quaternion::MakeAxisAngle(AXIS_Y, rot.y);

	Vec3 ansFrontV = vec;
	ansFrontV = qZ.GetRotateVector(ansFrontV);
	ansFrontV = qX.GetRotateVector(ansFrontV);
	ansFrontV = qY.GetRotateVector(ansFrontV);

	return ansFrontV;
}

bool GetFileNames(std::string folderPath, std::vector<std::string>& file_names)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	std::string searchName = folderPath + "\\*";
	wchar_t serchNameWC[128];
	ConstCharToWcharT(searchName.c_str(), serchNameWC);
	LPCWSTR serchNameW = serchNameWC;

	hFind = FindFirstFile(serchNameW, &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
		return false;
	}

	/* 指定のディレクトリ以下のファイル名をファイルがなくなるまで取得する */
	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			/* ディレクトリの場合は何もしない */
		}
		else {
			/* ファイルが見つかったらVector配列に保存する */
			char fileNameC[128];
			ConstWCharTToChar(win32fd.cFileName, fileNameC);
			std::string fileNameStr = fileNameC;
			file_names.push_back(fileNameStr);
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	return true;
}

float GetRand(float min, float max)
{
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_real_distribution<float> distr(min, max);
	return distr(eng);
}

Vec3 GetRandVec3(float min, float max)
{
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_real_distribution<float> distr(min, max);

	Vec3 randVec3 = { distr(eng),distr(eng) ,distr(eng) };

	return randVec3;
}

const DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
	DirectX::XMFLOAT4 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	result.w = lhs.w + rhs.w;
	return result;
}

const DirectX::XMFLOAT4 operator-(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
	DirectX::XMFLOAT4 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	result.w = lhs.w - rhs.w;
	return result;
}

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	DirectX::XMFLOAT3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	DirectX::XMFLOAT3 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	return result;
}

const DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	DirectX::XMFLOAT2 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	return result;
}

const DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	DirectX::XMFLOAT2 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	return result;
}

const DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, float RHS)
{
	DirectX::XMFLOAT4 result;
	result.x = lhs.x / RHS;
	result.y = lhs.y / RHS;
	result.z = lhs.z / RHS;
	result.w = lhs.w / RHS;
	return result;
}

const DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, float rhs)
{
	DirectX::XMFLOAT3 result;
	result.x = lhs.x * rhs;
	result.y = lhs.y * rhs;
	result.z = lhs.z * rhs;
	return result;
}

const DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, float rhs)
{
	DirectX::XMFLOAT2 result;
	result.x = lhs.x * rhs;
	result.y = lhs.y * rhs;
	return result;
}
