#include "Quaternion.h"
#include <math.h>

Quaternion Quaternion::GetMultiply(const Quaternion& rhs) const
{
	Quaternion ans;
	ans.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
	ans.x = y * rhs.z - z * rhs.y + rhs.w * x + w * rhs.x;
	ans.y = z * rhs.x - x * rhs.z + rhs.w * y + w * rhs.y;
	ans.z = x * rhs.y - y * rhs.x + rhs.w * z + w * rhs.z;
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

	if (length != 0.0f)
	{
		ans.w = w / length;
		ans.x = x / length;
		ans.y = y / length;
		ans.z = z / length;
	}

	return Quaternion(*this);
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
	Quaternion ans;
	Vec3 axis_ = axis;
	axis_.Normalized();

	float rad = sinf(angle / 2);
	ans.x = axis_.x * rad;
	ans.y = axis_.y * rad;
	ans.z = axis_.z * rad;
	ans.w = cosf(angle / 2);

	ans = ans.GetNormalize();

	return Quaternion(ans);
}

Quaternion Quaternion::DirectionToDirection(const Vec3& u, const Vec3& v, float dotRimitMin)
{
	Vec3 U = u;
	Vec3 V = v;

	//�����������������^�t�̎���y�����̓[���ɂ��ĉ񂳂���
	float dot = Vec3(U.x, U.y, U.z).Dot(Vec3(V.x, V.y, V.z));
	if (dot <= dotRimitMin)
	{
		U = { U.x,0,U.z };
		V = { V.x,0,V.z };
	}

	//u��v�𐳋K�����ē��ς����߂�
	dot = U.GetNormalized().Dot(V.GetNormalized());
	//u,v�̊O�ς��Ƃ�(��]�����o��)
	Vec3 cross = U.GetNormalized().Cross(V.GetNormalized());
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
	q = q.GetNormalize();
	Quaternion q2 = GetConjugate();
	r = q.GetMultiply(r);
	r = r.GetMultiply(q2);

	return Vec3(r.x, r.y, r.z);
}

M4 Quaternion::MakeRotateMatrix() const
{
	Quaternion q = GetNormalize();

	M4 ans = {
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
		2.0f * (q.x * q.y + q.w * q.z),
		2.0f * (q.x * q.z - q.w * q.y),
		0,

		2.0f * (q.x * q.y - q.w * q.z),
		q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
		2.0f * (q.y * q.z + q.w * q.x),
		0,

		2.0f * (q.x * q.z + q.w * q.y),
		2.0f * (q.y * q.z - q.w * q.x),
		q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
		0,

		0,0,0,1.0f
	};

	return M4(ans);
}

float Quaternion::DotQuaternion(const Quaternion& rhs) const
{
	float ans = (w * rhs.w + x * rhs.x + y * rhs.y + z * rhs.z);

	return ans;
}

void Quaternion::SetVec(const Vec3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
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

Quaternion SlerpUseVec3(const Vec3& v0, const Vec3& v1, float t)
{
	Quaternion q0;
	q0.SetVec(v0);
	Quaternion q1;
	q1.SetVec(v1);

	return Slerp(q0, q1, t);
}
