#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "M4.h"

//自作
void Vec4xM4(Vec4& v, const M4& m4);
void Vec3xM4(Vec3& v, const M4& m4, const bool w);

//ラジアン系
const float pi = 3.141592f;
float AngletoRadi(const float& angle);
float RaditoAngle(const float& radian);
