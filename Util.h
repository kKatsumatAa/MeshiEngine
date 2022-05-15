#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "M4.h"

//é©çÏ
void Vec4xM4(Vec4& v, const M4& m4);
void M4xM4(M4& m4, const M4& other);
void NormVec4(Vec4& v/*, const M4& m4*/);
void TransposeM4(M4& m4);


