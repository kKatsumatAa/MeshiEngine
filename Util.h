#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "M4.h"

//©ì
void Vec4xM4(Vec4& v, const M4& m4);
void Vec3xM4(Vec3& v, const M4& m4, const bool w);

//ƒ‰ƒWƒAƒ“Œn
const float pi = 3.141592f;
float AngletoRadi(const float& angle);
float RaditoAngle(const float& radian);
float GetRadianVec3(const Vec3& v1, const Vec3& v2);

//üŒ`•âŠ®
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t);
Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t);
