#include "Vec3.h"

float Vec3::GetLength() const
{
    return sqrtf(x_ * x_ + y_ * y_ + z_ * z_);
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

    v.x_ = y_ * other.z_ - z_ * other.y_;
    v.y_ = z_ * other.x_ - x_ * other.z_;
    v.z_ = x_ * other.y_ - y_ * other.x_;

    return v;
}

float Vec3::Dot(const Vec3& other)const
{
    return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}
//--------------------------------------------------------------


Vec3 Vec3::operator+()const
{
    return Vec3(*this);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x_ += other.x_;
    y_ += other.y_;
    z_ += other.z_;

    return *this;
}

Vec3 Vec3::operator-()const
{
    return Vec3({-x_,-y_,-z_});
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    x_ -= other.x_;
    y_ -= other.y_;
    z_ -= other.z_;

    return *this;
}

Vec3 Vec3::operator*(float other)
{
    Vec3 v = *this;
    v.x_ *= other;
    v.y_ *= other;
    v.z_ *= other;

    return Vec3(v);
}

Vec3& Vec3::operator*=(float other)
{
    x_ *= other;
    y_ *= other;
    z_ *= other;

    return *this;
}

Vec3 Vec3::operator/(float other)
{
    Vec3 v = *this;
    v.x_ /= other;
    v.y_ /= other;
    v.z_ /= other;

    return Vec3(v);
}

Vec3& Vec3::operator/=(float other)
{
    x_ /= other;
    y_ /= other;
    z_ /= other;

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
