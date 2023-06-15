#include "Quaternion.h"
#include <math.h>

Quaternion Quaternion::GetMultiply(const Quaternion& rhs) const
{
	Quaternion   ans;
	float   d1, d2, d3, d4;

	d1 = w * rhs.w;
	d2 = -x * rhs.x;
	d3 = -y * rhs.y;
	d4 = -z * rhs.z;
	ans.w = d1 + d2 + d3 + d4;


	d1 = y * rhs.z;
	d2 = -z * rhs.y;
	d3 = rhs.w * x;
	d4 = w * rhs.x;
	ans.x = d1 + d2 + d3 + d4;

	d1 = z * rhs.x;
	d2 = -x * rhs.z;
	d3 = rhs.w * y;
	d4 = w * rhs.y;
	ans.y = d1 + d2 + d3 + d4;

	d1 = x * rhs.y;
	d2 = y * rhs.x;
	d3 = rhs.w * z;
	d4 = w * rhs.z;
	ans.z = d1 + d2 + d3 + d4;

	return  Quaternion(ans);
}

Quaternion Quaternion::GetIdentityQuaternion()
{
	Quaternion ans(0.0f, 0.0f, 0.0f, 1.0f);

	return Quaternion(ans);
}

Quaternion Quaternion::GetConjugate() const
{
	Quaternion ans;

	ans.x = -x;
	ans.y = -y;
	ans.z = -z;
	ans.w = w;

	return Quaternion(ans);
}

float Quaternion::GetLength() const
{
	return sqrtf(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::GetNormalize() const
{
	Quaternion ans;

	float length = GetLength();

	ans.w = w / length;
	ans.x = x / length;
	ans.y = y / length;
	ans.z = z / length;

	return Quaternion(ans);
}

Quaternion Quaternion::GetInverse() const
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

Quaternion Quaternion::DirectionToDirection(const Vec3& u, const Vec3& v)
{
	//u��v�𐳋K�����ē��ς����߂�
	float dot = u.GetNormalized().Dot(v.GetNormalized());
	//u,v�̊O�ς��Ƃ�
	Vec3 cross = u.GetNormalized().Cross(v.GetNormalized());
	//���͒P�ʃx�N�g���ł���K�v������̂Ő��K��
	//u��v���P�ʃx�N�g�������Ă��A�O�ς��P�ʃx�N�g���Ƃ͌���Ȃ��̂ł����̐��K���͕K�{
	Vec3 axis = cross.GetNormalized();
	//�P�ʃx�N�g���œ��ς��Ƃ��Ă���̂�acos�Ŋp�x�����߂�
	float theta = acosf(dot);
	//axis��theta�ŔC�ӎ���]������ĕԂ�
	return MakeAxisAngle(axis, theta);
}

Vec3 Quaternion::GetRotateVector(const Vec3& vector) const
{
	Quaternion r = { vector.x,vector.y,vector.z,0 };
	Quaternion q = *this;
	Quaternion q2 = GetConjugate();
	Quaternion ans = q * r * q2;

	return Vec3(ans.x, ans.y, ans.z);
}

M4 Quaternion::MakeRotateMatrix() const
{
	M4 ans = {
		w * w + x * x - y * y - z * z,2 * (x * y + w * z),2 * (x * z - w * y),0,
		2 * (x * y - w * z),w * w - x * x + y * y - z * z,2 * (y * z + w * x),0,
		2 * (x * z + w * y),2 * (y * z - w * x),w * w - x * x - y * y + z * z,0,
		0,0,0,1
	};

	return M4(ans);
}

float Quaternion::DotQuaternion(const Quaternion& rhs) const
{
	float ans = (w * rhs.w + x * rhs.x + y * rhs.y + z * rhs.z);

	return ans;
}


//--------------------------------------------------------------
Quaternion Quaternion::operator+()
{
	return Quaternion(*this);
}

Quaternion& Quaternion::operator+=(const Quaternion& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

Quaternion Quaternion::operator-()const
{
	return Quaternion({ -x,-y,-z,-w });
}

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

Quaternion Quaternion::operator*(float other)
{
	Quaternion ans = *this;
	ans.x *= other;
	ans.y *= other;
	ans.z *= other;
	ans.w *= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator*=(float other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;

	return *this;
}

Quaternion Quaternion::operator/(float other)
{
	Quaternion ans = *this;
	ans.x /= other;
	ans.y /= other;
	ans.z /= other;
	ans.w /= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator/=(float other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;

	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& other)
{
	return Quaternion(GetMultiply(other));
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	*this = GetMultiply(other);

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

Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ans = q1;
	ans *= q2;

	return Quaternion(ans);
}


Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	float dot = q0.DotQuaternion(q1);

	Quaternion q02;

	if (dot < 0)
	{
		q02 = -q0; //�����Е��̉�]�𗘗p����
		dot = -dot; //���ς����]
	}
	else
	{
		q02 = q0;
	}

	//�Ȃ��p�����߂�
	float theta = acosf(dot);

	//theta��sin���g���ĕ�ԌW��scale0,scale1�����߂�
	float scale0 = sinf((1 - t) * theta) / sinf(theta);
	float scale1 = sinf(t * theta) / sinf(theta);

	//0���Z�ɂȂ�Ȃ����ǔ�
	if (dot >= 1.0f - FLT_EPSILON)
	{
		return (1.0f - t) * q02 + t * q1;
	}

	//���ꂼ��̕�ԌW���𗘗p���ĕ�Ԍ��Quaternion�����߂�
	return scale0 * q02 + scale1 * q1;	
}