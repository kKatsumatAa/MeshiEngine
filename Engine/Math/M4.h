#pragma once
#include"Vec3.h"


class M4
{
private://�G�C���A�X
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�s��
	double m_[4][4];

	static const float NORMAL_M[4][4];

public:
	static void CalcInvMat(M4& mat);

public:

	//�֐�
	M4();
	M4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	M4(const float(*other)[4]);
	/*void Normalize();*/

	//����

	void TransposeM4();

	//�s��Z�b�g
	void SetScaleMatrix(const Vec3& v3);
	void SetRotationMatrix(float angle, char axis);
	void SetTranslationMatrix(const Vec3& v3);

	void SetInverseMatrix();

	M4 GetInverseMatrix() const;

	//
	/*M4& operator=(const float (*other)[4]);*/
	M4 operator*(const M4& other);
	M4& operator*=(const M4& other);

	//���̃}�g���b�N�X�N���X��XMMATRIX�̒��g������
	void PutInXMMATRIX(const XMMATRIX& xM);
	//M4�̒��g��XMMATRIX�ɓ����
	void MatIntoXMMATRIX(XMMATRIX& xM)const;
};

const Vec3 operator*(const Vec3& v, const M4& m2);

