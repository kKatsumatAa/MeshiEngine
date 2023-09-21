#include "Vec3.h"

float Vec3::GetLength() const
{
    return sqrtf(x * x + y * y + z * z);
}

void Vec3::Normalized()
{
    float len = GetLength();
    if (len != 0.0f) {
        *this /= GetLength();
    }
}

Vec3 Vec3::GetNormalized() const
{
    float len = GetLength();
    if (len != 0.0f) { return *this / len; }

    return *this;
}

Vec3 Vec3::Cross(const Vec3& other)const
{
    Vec3 v;

    v.x = y * other.z - z * other.y;
    v.y = z * other.x - x * other.z;
    v.z = x * other.y - y * other.x;

    return v;
}

float Vec3::Dot(const Vec3& other)const
{
    return x * other.x + y * other.y + z * other.z;
}
//--------------------------------------------------------------


Vec3 Vec3::operator+()const
{
    return Vec3(*this);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3 Vec3::operator-()const
{
    return Vec3({-x,-y,-z});
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3 Vec3::operator*(float other)
{
    Vec3 v = *this;
    v.x *= other;
    v.y *= other;
    v.z *= other;

    return Vec3(v);
}

Vec3& Vec3::operator*=(float other)
{
    x *= other;
    y *= other;
    z *= other;

    return *this;
}

Vec3 Vec3::operator/(float other)
{
    Vec3 v = *this;
    v.x /= other;
    v.y /= other;
    v.z /= other;

    return Vec3(v);
}

Vec3& Vec3::operator/=(float other)
{
    x /= other;
    y /= other;
    z /= other;

    return *this;
}


//-------------------------------------

const Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    Vec3 temp(v1);
    return temp += v2;
}

const Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    Vec3 temp(v1);
    return temp -= v2;
}

const Vec3 operator*(const Vec3& v, float s)
{
    Vec3 temp(v);
    return temp *= s;
}

const Vec3 operator*(float s, const Vec3& v)
{
    Vec3 temp(v);
    return temp *= s;
}

const Vec3 operator/(const Vec3& v, float s)
{
    Vec3 temp(v);
    return temp /= s;
}
