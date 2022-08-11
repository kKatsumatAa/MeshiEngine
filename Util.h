#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "M4.h"
#include"DirectX.h"

//����
void Vec4xM4(Vec4& v, const M4& m4);
void Vec3xM4(Vec3& v, const M4& m4, const bool w);
void Vec3xM4andDivisionW(Vec3& v, const M4& m4, const bool w);

//���W�A���n
const float pi = 3.141592f;
float AngletoRadi(const float& angle);
float RaditoAngle(const float& radian);
float GetRadianVec3(const Vec3& v1, const Vec3& v2);

//���`�⊮
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t);
Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t);

//�Փ�
bool CollisionCircleCircle(const Vec3& pos1, const float& r1,
	const Vec3& pos2, const float& r2);

//�ϊ�
XMFLOAT2 Vec3toXMFLOAT2(const Vec3& v, const XMMATRIX& view, const XMMATRIX& projection);


