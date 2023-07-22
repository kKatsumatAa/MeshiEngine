#pragma once
#include"M4.h"

//�N�H�[�^�j�I��
class Quaternion
{
public://�ϐ�
	//v
	float x_;
	float y_;
	float z_;
	//w
	float w_;


public://�֐�
	Quaternion()
		: x_(0), y_(0), z_(0), w_(0) {};
	Quaternion(float x, float y, float z, float w)
		: x_(x), y_(y), z_(z), w_(w) {};


	//��
	Quaternion GetMultiply(const Quaternion& rhs) const;
	//�����N�H�[�^�j�I����Ԃ�
	Quaternion GetConjugate() const;
	//�N�H�[�^�j�I����Norm��Ԃ�
	float GetLength() const;
	//���K�������N�H�[�^�j�I����Ԃ�
	Quaternion GetNormalize() const;
	//�t�N�H�[�^�j�I����Ԃ�
	Quaternion GetInverse() const;


	//�x�N�g�����N�H�[�^�j�I���ŉ�]���������ʂ̃x�N�g�������߂�
	Vec3 GetRotateVector(const Vec3& vector) const;
	//�N�H�[�^�j�I�������]�s������߂�
	M4 MakeRotateMatrix() const;

	//����
	float DotQuaternion(const Quaternion& rhs)const;

	//x,y,z���Z�b�g
	void SetVec(const Vec3& vec);


public://�ÓI�����o�֐�
	//�P�ʃN�H�[�^�j�I���𐶐�
	static Quaternion GetIdentityQuaternion();

	//�C�ӎ���]��\���N�H�[�^�j�I����Ԃ�
	static Quaternion MakeAxisAngle(const Vec3& axis, float angle);

	//u����v�ւ̉�]�𐶐�
	static Quaternion DirectionToDirection(const Vec3& u, const Vec3& v, float dotRimitMin = -0.5f);


public://�I�o�����Z�q
	Quaternion  operator+();
	Quaternion& operator+=(const Quaternion& other);
	Quaternion  operator-()const;
	Quaternion& operator-=(const Quaternion& other);
	Quaternion  operator*(float other);
	Quaternion& operator*=(float other);
	Quaternion  operator/(float other);
	Quaternion& operator/=(float other);

	Quaternion  operator*(const Quaternion& other);
	Quaternion& operator*=(const Quaternion& other);
};



//�񍀉��Z�q�I�[�o�[���[�h
const Quaternion operator +(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator -(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator *(const Quaternion& v, float s);
const Quaternion operator *(float s, const Quaternion& v);
const Quaternion operator /(const Quaternion& v, float s);

Quaternion  operator*(const Quaternion& q1, const Quaternion& q2);



//���ʐ��`�⊮
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
Quaternion SlerpUseVec3(const Vec3& v0, const Vec3& v1, float t);



