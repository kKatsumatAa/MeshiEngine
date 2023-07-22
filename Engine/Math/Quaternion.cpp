#include "Quaternion.h"
#include <math.h>

Quaternion Quaternion::GetMultiply(const Quaternion& rhs) const
{
	Quaternion ans;
	ans.w_ = w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_;
	ans.x_ = y_ * rhs.z_ - z_ * rhs.y_ + rhs.w_ * x_ + w_ * rhs.x_;
	ans.y_ = z_ * rhs.x_ - x_ * rhs.z_ + rhs.w_ * y_ + w_ * rhs.y_;
	ans.z_ = x_ * rhs.y_ - y_ * rhs.x_ + rhs.w_ * z_ + w_ * rhs.z_;
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

	ans.x_ = -x_;
	ans.y_ = -y_;
	ans.z_ = -z_;
	ans.w_ = w_;

	return Quaternion(ans);
}

float Quaternion::GetLength() const
{
	return sqrtf(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);
}

Quaternion Quaternion::GetNormalize() const
{
	Quaternion ans;

	float length = GetLength();

	if (length != 0.0f)
	{
		ans.w_ = w_ / length;
		ans.x_ = x_ / length;
		ans.y_ = y_ / length;
		ans.z_ = z_ / length;
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
	ans.x_ = axis_.x_ * rad;
	ans.y_ = axis_.y_ * rad;
	ans.z_ = axis_.z_ * rad;
	ans.w_ = cosf(angle / 2);

	ans = ans.GetNormalize();

	return Quaternion(ans);
}

Quaternion Quaternion::DirectionToDirection(const Vec3& u, const Vec3& v, float dotRimitMin)
{
	Vec3 U = u;
	Vec3 V = v;

	//向かせたい向きが真逆の時はy成分はゼロにして回させる
	float dot = Vec3(U.x_, U.y_, U.z_).Dot(Vec3(V.x_, V.y_, V.z_));
	if (dot <= dotRimitMin)
	{
		U = { U.x_,0,U.z_ };
		V = { V.x_,0,V.z_ };
	}

	//uとvを正規化して内積を求める
	dot = U.GetNormalized().Dot(V.GetNormalized());
	//u,vの外積をとる(回転軸を出す)
	Vec3 cross = U.GetNormalized().Cross(V.GetNormalized());
	//軸は単位ベクトルである必要があるので正規化
	//uとvが単位ベクトルあっても、外積が単位ベクトルとは限らないのでここの正規化は必須
	Vec3 axis = cross.GetNormalized();
	//単位ベクトルで内積をとっているのでacosで角度を求める
	float theta = acosf(dot);
	//axisとthetaで任意軸回転を作って返す
	return MakeAxisAngle(axis, theta);
}

Vec3 Quaternion::GetRotateVector(const Vec3& vector) const
{
	Quaternion r = { vector.x_,vector.y_,vector.z_,0 };
	Quaternion q = *this;
	q = q.GetNormalize();
	Quaternion q2 = GetConjugate();
	r = q.GetMultiply(r);
	r = r.GetMultiply(q2);

	return Vec3(r.x_, r.y_, r.z_);
}

M4 Quaternion::MakeRotateMatrix() const
{
	Quaternion q = GetNormalize();

	M4 ans = {
		q.w_ * q.w_ + q.x_ * q.x_ - q.y_ * q.y_ - q.z_ * q.z_,
		2.0f * (q.x_ * q.y_ + q.w_ * q.z_),
		2.0f * (q.x_ * q.z_ - q.w_ * q.y_),
		0,

		2.0f * (q.x_ * q.y_ - q.w_ * q.z_),
		q.w_ * q.w_ - q.x_ * q.x_ + q.y_ * q.y_ - q.z_ * q.z_,
		2.0f * (q.y_ * q.z_ + q.w_ * q.x_),
		0,

		2.0f * (q.x_ * q.z_ + q.w_ * q.y_),
		2.0f * (q.y_ * q.z_ - q.w_ * q.x_),
		q.w_ * q.w_ - q.x_ * q.x_ - q.y_ * q.y_ + q.z_ * q.z_,
		0,

		0,0,0,1.0f
	};

	return M4(ans);
}

float Quaternion::DotQuaternion(const Quaternion& rhs) const
{
	float ans = (w_ * rhs.w_ + x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_);

	return ans;
}

void Quaternion::SetVec(const Vec3& vec)
{
	x_ = vec.x_;
	y_ = vec.y_;
	z_ = vec.z_;
}


//--------------------------------------------------------------
Quaternion Quaternion::operator+()
{
	return Quaternion(*this);
}

Quaternion& Quaternion::operator+=(const Quaternion& other)
{
	x_ += other.x_;
	y_ += other.y_;
	z_ += other.z_;
	w_ += other.w_;

	return *this;
}

Quaternion Quaternion::operator-()const
{
	return Quaternion({ -x_,-y_,-z_,-w_ });
}

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
	x_ -= other.x_;
	y_ -= other.y_;
	z_ -= other.z_;
	w_ -= other.w_;

	return *this;
}

Quaternion Quaternion::operator*(float other)
{
	Quaternion ans = *this;
	ans.x_ *= other;
	ans.y_ *= other;
	ans.z_ *= other;
	ans.w_ *= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator*=(float other)
{
	x_ *= other;
	y_ *= other;
	z_ *= other;
	w_ *= other;

	return *this;
}

Quaternion Quaternion::operator/(float other)
{
	Quaternion ans = *this;
	ans.x_ /= other;
	ans.y_ /= other;
	ans.z_ /= other;
	ans.w_ /= other;

	return Quaternion(ans);
}

Quaternion& Quaternion::operator/=(float other)
{
	x_ /= other;
	y_ /= other;
	z_ /= other;
	w_ /= other;

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
		q02 = -q0; //もう片方の回転を利用する
		dot = -dot; //内積も反転
	}
	else
	{
		q02 = q0;
	}

	//なす角を求める
	float theta = acosf(dot);

	//thetaとsinを使って補間係数scale0,scale1を求める
	float scale0 = sinf((1 - t) * theta) / sinf(theta);
	float scale1 = sinf(t * theta) / sinf(theta);

	//0除算にならない改良版
	if (dot >= 1.0f - FLT_EPSILON)
	{
		return (1.0f - t) * q02 + t * q1;
	}

	//それぞれの補間係数を利用して補間後のQuaternionを求める
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
