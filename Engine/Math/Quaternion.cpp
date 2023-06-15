#include "Quaternion.h"
#include <math.h>

Quaternion Quaternion::GetMultiply(const Quaternion& rhs) const
{
	Quaternion   ans;
	float   d1, d2, d3, d4;

	d1 = w_ * rhs.w_;
	d2 = -x_ * rhs.x_;
	d3 = -y_ * rhs.y_;
	d4 = -z_ * rhs.z_;
	ans.w_ = d1 + d2 + d3 + d4;


	d1 = y_ * rhs.z_;
	d2 = -z_ * rhs.y_;
	d3 = rhs.w_ * x_;
	d4 = w_ * rhs.x_;
	ans.x_ = d1 + d2 + d3 + d4;

	d1 = z_ * rhs.x_;
	d2 = -x_ * rhs.z_;
	d3 = rhs.w_ * y_;
	d4 = w_ * rhs.y_;
	ans.y_ = d1 + d2 + d3 + d4;

	d1 = x_ * rhs.y_;
	d2 = y_ * rhs.x_;
	d3 = rhs.w_ * z_;
	d4 = w_ * rhs.z_;
	ans.z_ = d1 + d2 + d3 + d4;

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

	ans.w_ = w_ / length;
	ans.x_ = x_ / length;
	ans.y_ = y_ / length;
	ans.z_ = z_ / length;

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
	Quaternion ans(ansV.x_, ansV.y_, ansV.z_, cosf(angle / 2.0f));

	return Quaternion(ans);
}

Quaternion Quaternion::DirectionToDirection(const Vec3& u, const Vec3& v)
{
	//uとvを正規化して内積を求める
	float dot = u.GetNormalized().Dot(v.GetNormalized());
	//u,vの外積をとる
	Vec3 cross = u.GetNormalized().Cross(v.GetNormalized());
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
	Quaternion q2 = GetConjugate();
	Quaternion ans = q * r * q2;

	return Vec3(ans.x_, ans.y_, ans.z_);
}

M4 Quaternion::MakeRotateMatrix() const
{
	M4 ans = {
		w_ * w_ + x_ * x_ - y_ * y_ - z_ * z_,2 * (x_ * y_ + w_ * z_),2 * (x_ * z_ - w_ * y_),0,
		2 * (x_ * y_ - w_ * z_),w_ * w_ - x_ * x_ + y_ * y_ - z_ * z_,2 * (y_ * z_ + w_ * x_),0,
		2 * (x_ * z_ + w_ * y_),2 * (y_ * z_ - w_ * x_),w_ * w_ - x_ * x_ - y_ * y_ + z_ * z_,0,
		0,0,0,1
	};

	return M4(ans);
}

float Quaternion::DotQuaternion(const Quaternion& rhs) const
{
	float ans = (w_ * rhs.w_ + x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_);

	return ans;
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