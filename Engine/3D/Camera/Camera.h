#pragma once
#include"ViewMat.h"
#include"ProjectionMat.h"
#include"Shake.h"

#include <DirectXMath.h>

/// <summary>
/// �J������{�@�\
/// </summary>
class Camera
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �����o�֐�
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	Camera();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Camera() = default;

	virtual void Initialize();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �r���[�s����X�V
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// �ˉe�s����X�V
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// �r���{�[�h�s��̎擾
	/// </summary>
	/// <returns>�r���{�[�h�s��</returns>
	inline const XMMATRIX& GetBillboardMatrix() {
		return matBillboard_;
	}

	/// <summary>
	/// ���_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	inline const Vec3& GetEye() const{
		return viewMat_.eye_;
	}

	/// <summary>
	/// ���_���W�̐ݒ�
	/// </summary>
	/// <param name="eye">���W</param>
	inline void SetEye(const Vec3& eye) {
		viewMat_.eye_ = eye; viewDirty_ = true;
	}

	/// <summary>
	/// �����_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	inline const Vec3& GetTarget() const {
		return viewMat_.target_;
	}

	/// <summary>
	/// �����_���W�̐ݒ�
	/// </summary>
	/// <param name="target">���W</param>
	inline void SetTarget(const Vec3& target) {
		viewMat_.target_ = target; viewDirty_ = true;
	}

	/// <summary>
	/// ������x�N�g���̎擾
	/// </summary>
	/// <returns>������x�N�g��</returns>
	inline const Vec3& GetUp() const {
		return viewMat_.up_;
	}

	/// <summary>
	/// ������x�N�g���̐ݒ�
	/// </summary>
	/// <param name="up">������x�N�g��</param>
	inline void SetUp(const Vec3& up) {
		viewMat_.up_ = up; viewDirty_ = true;
	}

	/// <summary>
	/// �x�N�g���ɂ�鎋�_�ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	void MoveEyeVector(const Vec3& move);
	void MoveEyeVector(const XMVECTOR& move);

	//
	void CameraShake(int32_t time, float length);

	/// <summary>
	/// �x�N�g���ɂ��ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	void MoveVector(const Vec3& move);
	void MoveVector(const XMVECTOR& move);

protected: // �����o�ϐ�
	Shake shake_;

	// �r���{�[�h�s��
	XMMATRIX matBillboard_ = DirectX::XMMatrixIdentity();
	// Y�����r���{�[�h�s��
	XMMATRIX matBillboardY_ = DirectX::XMMatrixIdentity();
	//����
	XMMATRIX matViewProjection_ = DirectX::XMMatrixIdentity();
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty_ = false;
	// �ˉe�s��_�[�e�B�t���O
	bool projectionDirty_ = false;

private:
	ViewMat viewMat_;
	ProjectionMat projectionMat_;

public:
	const XMMATRIX& GetViewMat()const { return viewMat_.matView_; }
	const XMMATRIX& GetProjMat()const { return projectionMat_.matProjection_; }
	const XMMATRIX& GetBillboardMat()const { return matBillboard_; }
	const XMMATRIX& GetBillboardMatY()const { return matBillboardY_; }
};



