#pragma once

#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "M4.h"
#include "Quaternion.h"
#include"TextureManager.h"


//����
void Vec4xM4(Vec4& v, const M4& m4);
void Vec3xM4(Vec3& v, const M4& m4, bool w);
Vec3 GetVec3xM4(const Vec3& v, const M4& m4, bool w);
void Vec3xM4andDivisionW(Vec3& v, const M4& m4, bool w);

//--------------------------------------------------------------------------------------------------
//���W�A���n
float AngletoRadi(float angle);
float RaditoAngle(float radian);
float GetRadianVec3(const Vec3& v1, const Vec3& v2);

//--------------------------------------------------------------------------------------------------
//���`�⊮
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t);
Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t);

//�X�v���C�����
Vec3 SplinePosition(const std::vector<Vec3>& points, int32_t startIndex, float t);

//�C�[�W���O
float EaseIn(float t);
float EaseOut(float t);
float EaseInOutBack(float t);
float EaseInOut(float t);

//--------------------------------------------------------------------------------------------------
//�Փ�
bool CollisionCircleCircle(const Vec3& pos1, float r1,
	const Vec3& pos2, float r2);

bool CollisionRayCircle(const Vec3& sv, const Vec3& ev, float r, const Vec3& pos, float r2);

bool CollisionBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t r1, int32_t r2);
bool CollisionBox(Vec2 leftUp1, Vec2 rightBottom1, Vec2 leftUp2, Vec2 rightBottom2);

//--------------------------------------------------------------------------------------------------
//�֗�
/// <summary>
/// 1���]1�ŕԂ��i0��0�j
/// </summary>
/// <returns></returns>
float sign(float num);

//--------------------------------------------------------------------------------------------------
//�ϊ�
Vec2 Vec3toVec2(const Vec3& v, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
/// <summary>
/// distance�́Aprojection��nearPos����̈ʒu!!
/// </summary>
/// <param name="v"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
/// <param name="distance"></param>
/// <returns></returns>
Vec3 Vec2toVec3(const Vec2& v, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, float distance);

/// <summary>
/// �X�N���[�����W��projection��near,far�̈ʒu�ɕϊ�����return...Pos�ɑ��
/// </summary>
/// <param name="v"></param>
/// <param name="returnNearPos"></param>
/// <param name="returnFarPos"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
void Vec2toNearFarPos(const Vec2& pos, Vec3& returnNearPos, Vec3& returnFarPos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

//--------------------------------------------------------------------------------------------------
//�A���C�����g�ɂ��낦���T�C�Y��Ԃ�
int32_t AligmentSize(int32_t size, int32_t aligment);

//-----------------------------------------------------------
//�����ϊ��n
//char* �� const wchar_t* �ɕϊ�
void ConstCharToWcharT(const char* string, wchar_t(&wString)[128]);

//const wchar_t* �� char* �ɕϊ�
void ConstWCharTToChar(const wchar_t* wString, char(&string)[128]);

//�����񂩂�findChar�ȊO�𔲂����(���̕���������̂�)
std::string MySubstr(const std::string& str, char findChar);

//�f�B���N�g�����܂񂾃t�@�C���p�X����t�@�C����(***.pmg)�𒊏o����
std::string ExtractFileName(const std::string& path);

//-------------------------------------------------------------
//�X���[�X�X�e�b�v
float SmoothStep(float min, float max, float v);

//�N�����v
float Clamp(float v,float min, float max);

//�����_��
float GetRand(float min, float max);

//------------------------------------------------------------------------
//��]�s�񂩂�p�x�����߂�
Vec3 GetRotFromQuaternion(Quaternion q);
Vec3 GetRotFromMat(M4 m);
//�قړ��������ǂ���
bool Approximately(float a, float b);

//�x�N�g�����p�x���g���ăN�H�[�^�j�I���ŉ�]
Vec3 GetTurnVec3UseQuaternionAndRot(const Vec3& vec, const Vec3& rot);

//-----------------------------------------------------------------------
//�t�H���_�n
bool GetFileNames(std::string folderPath, std::vector<std::string>& file_names);


//--------------------------------------------------------------------------------------------------
const DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs);

const DirectX::XMFLOAT4 operator-(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs);

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);

const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);

const DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

const DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

const DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, float RHS);

const DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, float rhs);

const DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, float rhs);