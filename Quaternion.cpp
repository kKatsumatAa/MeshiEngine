#include "Quaternion.h"
#include <math.h>

Quaternion Quaternion::GetMultiply(const Quaternion& rhs)
{
	Quaternion   ans;
	float   d1, d2, d3, d4;

	d1 = this->w * rhs.w;
	d2 = -this->x * rhs.x;
	d3 = -this->y * rhs.y;
	d4 = -this->z * rhs.z;
	ans.w = d1 + d2 + d3 + d4;


	d1 = this->y * rhs.z;
	d2 = -this->z * rhs.y;
	d3 = rhs.w * this->x;
	d4 = this->w * rhs.x;
	ans.x = d1 + d2 + d3 + d4;

	d1 = this->z * rhs.x;
	d2 = -this->x * rhs.z;
	d3 = rhs.w * this->y;
	d4 = this->w * rhs.y;
	ans.y = d1 + d2 + d3 + d4;

	d1 = this->x * rhs.y;
	d2 = this->y * rhs.x;
	d3 = rhs.w * this->z;
	d4 = this->w * rhs.z;
	ans.z = d1 + d2 + d3 + d4;

	return  Quaternion(ans);
}

Quaternion Quaternion::GetIdentityQuaternion()
{
	Quaternion ans(0.0f, 0.0f, 0.0f, 1.0f);

	return Quaternion(ans);
}

Quaternion Quaternion::GetConjugate()
{
	Quaternion ans;

	ans.x = -this->x;
	ans.y = -this->y;
	ans.z = -this->z;
	ans.w = this->w;

	return Quaternion(ans);
}

float Quaternion::GetLength()
{
	return sqrtf(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::GetNormalize()
{
	Quaternion ans;

	float length = GetLength();

	ans.w = this->w / length;
	ans.x = this->x / length;
	ans.y = this->y / length;
	ans.z = this->z / length;

	return Quaternion(ans);
}

Quaternion Quaternion::GetInverse()
{
	Quaternion ans;

	ans = GetConjugate() / (GetLength() * GetLength());

	return Quaternion(ans);
}


//----------
Quaternion Quaternion::MakeAxisAngle(const Vec3& axis, float angle)
{
	Vec3 ansV = axis.GetNormalized() * sinf(angle / 2.0f);
	Quaternion ans(ansV.x, ansV.y, ansV.z, cosf(angle / 2.0f));

	return Quaternion(ans);
}

Vec3 Quaternion::GetRotateVector(const Vec3& vector)
{
	Quaternion r = { vector.x,vector.y,vector.z,0 };
	Quaternion q = *this;
	Quaternion q2 = this->GetConjugate();
	Quaternion ans = q * r * q2;

	return Vec3(ans.x, ans.y, ans.z);
}

M4 Quaternion::MakeRotateMatrix()
{
	M4 ans = {
		w * w + x * x - y * y - z * z,2 * (x * y + w * z),2 * (x * z - w * y),0,
		2 * (x * y - w * z),w * w - x * x + y * y - z * z,2 * (y * z + w * x),0,
		2 * (x * z + w * y),2 * (y * z - w * x),w * w - x * x - y * y + z * z,0,
		0,0,0,1
	};

	return M4(ans);
}


//--------------------------------------------------------------
Quaternion Quaternion::operator+()
{
	return Quaternion(*this);
}

Quaternion& Quaternion::operator+=(const Quaternion& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	this->w += other.w;

	return *this;
}

Quaternion Quaternion::operator-()
{
	return Quaternion({ -x,-y,-z,-w });
}

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	this->w -= other.w;

	return *this;
}

Quaternion Quaternion::operator*(const float& other)
{
	Quaternion ans = *this;
	ans.x *= other;
	ans.y *= other;
	ans.z *= other;
	ans.w *= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator*=(const float& other)
{
	this->x *= other;
	this->y *= other;
	this->z *= other;
	this->w *= other;

	return *this;
}

Quaternion Quaternion::operator/(const float& other)
{
	Quaternion ans = *this;
	ans.x /= other;
	ans.y /= other;
	ans.z /= other;
	ans.w /= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator/=(const float& other)
{
	this->x /= other;
	this->y /= other;
	this->z /= other;
	this->w /= other;

	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& other)
{
	return Quaternion(this->GetMultiply(other));
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	*this = this->GetMultiply(other);

	return *this;
}


//-------------------------------------

const Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ans(q1);
	return ans += q2;
}

const Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ans(q1);
	return ans -= q2;
}

const Quaternion operator*(const Quaternion& q, float s)
{
	Quaternion ans(q);
	return ans *= s;
}

const Quaternion operator*(float s, const Quaternion& q)
{
	Quaternion ans(q);
	return ans *= s;
}

const Quaternion operator/(const Quaternion& q, float s)
{
	Quaternion ans(q);
	return ans /= s;
}
