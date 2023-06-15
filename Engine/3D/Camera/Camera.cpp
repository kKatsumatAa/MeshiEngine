#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	//�r���[�s��̌v�Z
	UpdateViewMatrix();

	// �ˉe�s��̌v�Z
	UpdateProjectionMatrix();

	// �r���[�v���W�F�N�V�����̍���
	matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;
}

void Camera::Initialize()
{
	//�r���[�s��̌v�Z
	UpdateViewMatrix();

	// �ˉe�s��̌v�Z
	UpdateProjectionMatrix();

	// �r���[�v���W�F�N�V�����̍���
	matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;

	shake_.Initialize();
}

void Camera::Update()
{
	if (viewDirty_ || projectionDirty_ || shake_.GetIsShaking()) {
		// �Čv�Z�K�v�Ȃ�
		if (viewDirty_ || shake_.GetIsShaking()) {
			// �r���[�s��X�V
			UpdateViewMatrix();
			viewDirty_ = false;
		}

		// �Čv�Z�K�v�Ȃ�
		if (projectionDirty_) {
			// �r���[�s��X�V
			UpdateProjectionMatrix();
			projectionDirty_ = false;
		}
		// �r���[�v���W�F�N�V�����̍���
		matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;
	}
}

void Camera::UpdateViewMatrix()
{
	//shake_
	shake_.Update();
	float shakeNum = shake_.GetShake();
	// ���_���W
	XMVECTOR eyePosition = { viewMat_.eye_.x_ + shakeNum,viewMat_.eye_.y_ + shakeNum,viewMat_.eye_.z_ + shakeNum };
	// �����_���W
	XMVECTOR targetPosition = { viewMat_.target_.x_ + shakeNum,viewMat_.target_.y_ + shakeNum,viewMat_.target_.z_ + shakeNum };
	// �i���́j�����
	XMVECTOR upVector = { viewMat_.up_.x_,viewMat_.up_.y_,viewMat_.up_.z_ };

	// �J����Z���i���������j
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	// 0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	// �x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// �J������X���i�E�����j
	XMVECTOR cameraAxisX;
	// X���͏������Z���̊O�ςŋ��܂�
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// �x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// �J������Y���i������j
	XMVECTOR cameraAxisY;
	// Y����Z����X���̊O�ςŋ��܂�
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// �����܂łŒ�������3�����̃x�N�g��������
	//�i���[���h���W�n�ł̃J�����̉E�����A������A�O�����j	

	// �J������]�s��
	XMMATRIX matCameraRot;
	// �J�������W�n�����[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// �]�u�ɂ��t�s��i�t��]�j���v�Z
	viewMat_.matView_ = XMMatrixTranspose(matCameraRot);

	// ���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// �J�����̈ʒu���烏�[���h���_�ւ̃x�N�g���i�J�������W�n�j
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	// X����
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	// Y����
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	// Z����
	// ��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// �r���[�s��ɕ��s�ړ�������ݒ�
	viewMat_.matView_.r[3] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	// �r���{�[�h�s��
	matBillboard_.r[0] = cameraAxisX;
	matBillboard_.r[1] = cameraAxisY;
	matBillboard_.r[2] = cameraAxisZ;
	matBillboard_.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

#pragma region Y�����r���{�[�h�s��̌v�Z
	// �J����X���AY���AZ��
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X���͋���
	ybillCameraAxisX = cameraAxisX;
	// Y���̓��[���h���W�n��Y��
	ybillCameraAxisY = XMVector3Normalize(upVector);
	// Z����X����Y���̊O�ςŋ��܂�
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y�����r���{�[�h�s��
	matBillboardY_.r[0] = ybillCameraAxisX;
	matBillboardY_.r[1] = ybillCameraAxisY;
	matBillboardY_.r[2] = ybillCameraAxisZ;
	matBillboardY_.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion
}

void Camera::UpdateProjectionMatrix()
{
	// �������e�ɂ��ˉe�s��̐���
	projectionMat_.SetMat();
}

void Camera::MoveEyeVector(const Vec3& move)
{
	// ���_���W���ړ����A���f
	Vec3 eye_moved = GetEye();

	eye_moved.x_ += move.x_;
	eye_moved.y_ += move.y_;
	eye_moved.z_ += move.z_;

	SetEye(eye_moved);
}

void Camera::MoveEyeVector(const XMVECTOR& move)
{
	// ���_���W���ړ����A���f
	Vec3 eye_moved = GetEye();

	eye_moved.x_ += move.m128_f32[0];
	eye_moved.y_ += move.m128_f32[1];
	eye_moved.z_ += move.m128_f32[2];

	SetEye(eye_moved);
}

void Camera::CameraShake(int32_t time, float length)
{
	shake_.SetShake(time, length);
}

void Camera::MoveVector(const Vec3& move)
{
	// ���_�ƒ����_���W���ړ����A���f
	Vec3 eye_moved = GetEye();
	Vec3 target_moved = GetTarget();

	eye_moved.x_ += move.x_;
	eye_moved.y_ += move.y_;
	eye_moved.z_ += move.z_;

	target_moved.x_ += move.x_;
	target_moved.y_ += move.y_;
	target_moved.z_ += move.z_;

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void Camera::MoveVector(const XMVECTOR& move)
{
	// ���_�ƒ����_���W���ړ����A���f
	Vec3 eye_moved = GetEye();
	Vec3 target_moved = GetTarget();

	eye_moved.x_ += move.m128_f32[0];
	eye_moved.y_ += move.m128_f32[1];
	eye_moved.z_ += move.m128_f32[2];

	target_moved.x_ += move.m128_f32[0];
	target_moved.y_ += move.m128_f32[1];
	target_moved.z_ += move.m128_f32[2];

	SetEye(eye_moved);
	SetTarget(target_moved);
}
