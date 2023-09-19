#include "Camera2D.h"
#include "Util.h"

using namespace DirectX;

void Camera2D::Initialize()
{
}

void Camera2D::Update()
{
	FollowUpdate();

	ZoomUpdate();

	//シェイク更新
	shake_.Update();

	//見える範囲更新
	VisiableUpdate();

	//行列更新
	MatrixUpdate();
}

void Camera2D::VisiableUpdate()
{
	visiableArea_.x = pos_.x - visiableArea_.z / 2.0f;
	visiableArea_.y = pos_.y - visiableArea_.w / 2.0f;
}

void Camera2D::MatrixUpdate()
{
	//3Dに変換
	Vec3 pos = { pos_.x + shake_.GetShake(),pos_.y + shake_.GetShake(),0 };
	Vec3 screenPos = { screenCenterPos_.x,screenCenterPos_.y,0 };

	//行列作成
	cameraMatrix_ = XMMatrixIdentity();
	cameraMatrix_ *= XMMatrixTranslation(-pos.x, -pos.y, -pos.z);
	cameraMatrix_ *= XMMatrixScaling(zoom_.x, zoom_.y, 1.0f);
	cameraMatrix_ *= XMMatrixRotationZ(rot_);
	cameraMatrix_ *= XMMatrixTranslation(screenPos.x, screenPos.y, screenPos.z);
}

void Camera2D::FollowUpdate()
{
	if (isFollow_)
	{
		float length = fabsf((pos_ - oldPos_).GetLength());
		Vec2 dir = (pos_ - oldPos_).GetNormalize();

		followT_ = min(followT_ + length / followLengthMax_ / 9.0f, 1.0f);

		Vec2 offset = LerpVec2({ 0,0 }, dir * followLengthMax_, EaseIn(followT_));

		if ((pos_ - (oldPos_ + offset)).GetNormalize().Dot((pos_ - oldPos_).GetNormalize()) < 0.0f)
		{
			SetPos(pos_);
			followT_ = 0;
		}
		else
		{
			pos_ = oldPos_ + offset;
		}
		oldPos_ = pos_;
		//followT_ += 1.0f / 10.0f;
	}
}

void Camera2D::ZoomUpdate()
{
	if (isZoom_)
	{
		float t = min((float)zoomTimer_ / (float)zoomTimerMax_, 1.0f);

		float lerpZoom = LerpVec2({ startZoomP_,0 }, { endZoomP_,0 }, EaseIn(t)).x;

		zoom_ = { lerpZoom ,lerpZoom };

		if (t >= 1.0f)
		{
			isZoom_ = false;
		}

		zoomTimer_++;
	}
}

//----------------------------------------------------------
void Camera2D::BeginShake(uint32_t time, float maxLength)
{
	shake_.SetShake(time, maxLength);
}

void Camera2D::BeginFollow(float maxLength)
{
	//追従
	isFollow_ = true;
	//追従用
	followLengthMax_ = maxLength;
	//割合
	followT_ = 0;
	//前の座標
	oldPos_ = pos_;
}

void Camera2D::EndFollow()
{
	isFollow_ = false;
}


//-----------------------------------------------------------
void Camera2D::BeginZoom(float endZoomPow, int32_t time)
{
	startZoomP_ = (zoom_.x + zoom_.y) / 2.0f;
	endZoomP_ = endZoomPow;
	zoomTimerMax_ = time;
	zoomTimer_ = 0;
	isZoom_ = true;
}

