#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	//ビュー行列の計算
	UpdateViewMatrix();

	// 射影行列の計算
	UpdateProjectionMatrix();

	// ビュープロジェクションの合成
	matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;
}

void Camera::Initialize()
{
	//ビュー行列の計算
	UpdateViewMatrix();

	// 射影行列の計算
	UpdateProjectionMatrix();

	// ビュープロジェクションの合成
	matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;

	shake_.Initialize();
}

void Camera::Update()
{
	if (viewDirty_ || projectionDirty_ || shake_.GetIsShaking()) {
		// 再計算必要なら
		if (viewDirty_ || shake_.GetIsShaking()) {
			// ビュー行列更新
			UpdateViewMatrix();
			viewDirty_ = false;
		}

		// 再計算必要なら
		if (projectionDirty_) {
			// ビュー行列更新
			UpdateProjectionMatrix();
			projectionDirty_ = false;
		}
		// ビュープロジェクションの合成
		matViewProjection_ = viewMat_.matView_ * projectionMat_.matProjection_;
	}
}

void Camera::UpdateViewMatrix()
{
	//shake_
	shake_.Update();
	float shakeNum = shake_.GetShake();
	// 視点座標
	XMVECTOR eyePosition = { viewMat_.eye_.x_ + shakeNum,viewMat_.eye_.y_ + shakeNum,viewMat_.eye_.z_ + shakeNum };
	// 注視点座標
	XMVECTOR targetPosition = { viewMat_.target_.x_ + shakeNum,viewMat_.target_.y_ + shakeNum,viewMat_.target_.z_ + shakeNum };
	// （仮の）上方向
	XMVECTOR upVector = { viewMat_.up_.x_,viewMat_.up_.y_,viewMat_.up_.z_ };

	// カメラZ軸（視線方向）
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	// 0ベクトルだと向きが定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	// ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// カメラのX軸（右方向）
	XMVECTOR cameraAxisX;
	// X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// カメラのY軸（上方向）
	XMVECTOR cameraAxisY;
	// Y軸はZ軸→X軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// ここまでで直交した3方向のベクトルが揃う
	//（ワールド座標系でのカメラの右方向、上方向、前方向）	

	// カメラ回転行列
	XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// 転置により逆行列（逆回転）を計算
	viewMat_.matView_ = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// カメラの位置からワールド原点へのベクトル（カメラ座標系）
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	// X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	// Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	// Z成分
	// 一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// ビュー行列に平行移動成分を設定
	viewMat_.matView_.r[3] = translation;

#pragma region 全方向ビルボード行列の計算
	// ビルボード行列
	matBillboard_.r[0] = cameraAxisX;
	matBillboard_.r[1] = cameraAxisY;
	matBillboard_.r[2] = cameraAxisZ;
	matBillboard_.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

#pragma region Y軸回りビルボード行列の計算
	// カメラX軸、Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X軸は共通
	ybillCameraAxisX = cameraAxisX;
	// Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	// Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y軸回りビルボード行列
	matBillboardY_.r[0] = ybillCameraAxisX;
	matBillboardY_.r[1] = ybillCameraAxisY;
	matBillboardY_.r[2] = ybillCameraAxisZ;
	matBillboardY_.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion
}

void Camera::UpdateProjectionMatrix()
{
	// 透視投影による射影行列の生成
	projectionMat_.SetMat();
}

void Camera::MoveEyeVector(const Vec3& move)
{
	// 視点座標を移動し、反映
	Vec3 eye_moved = GetEye();

	eye_moved.x_ += move.x_;
	eye_moved.y_ += move.y_;
	eye_moved.z_ += move.z_;

	SetEye(eye_moved);
}

void Camera::MoveEyeVector(const XMVECTOR& move)
{
	// 視点座標を移動し、反映
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
	// 視点と注視点座標を移動し、反映
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
	// 視点と注視点座標を移動し、反映
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
