#pragma once
#include "Vec3.h"
#include "M4.h"

//é©çÏ
void Vec3xM4(Vec3& v, const M4& m4);
void M4xM4(M4& m4, const M4& other);
void NormVec3(Vec3& v/*, const M4& m4*/);
void TransposeM4(M4& m4);


