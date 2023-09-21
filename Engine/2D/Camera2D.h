#pragma once
#include"ViewMat.h"
#include"ProjectionMat.h"
#include"Shake.h"

#include <DirectXMath.h>

//2D�p�̃J����
class Camera2D
{
private:
	//�ʒu
	Vec2 pos_ = { WindowsApp::WINDOW_WIDTH_ / 2.0f,WindowsApp::WINDOW_HEIGHT_ / 2.0f };
	//�g�嗦
	Vec2 zoom_ = { 1.0f,1.0f };
	//������͈�
	Vec4 visiableArea_ = { 0,0,WindowsApp::WINDOW_WIDTH_,WindowsApp::WINDOW_HEIGHT_ };
	//�p�x
	float rot_ = 0.0f;
	//�X�N���[�����S
	Vec2 screenCenterPos_ = { WindowsApp::WINDOW_WIDTH_ / 2.0f,WindowsApp::WINDOW_HEIGHT_ / 2.0f };
	//�J�����s��
	DirectX::XMMATRIX cameraMatrix_;
	//�V�F�C�N
	Shake shake_;
	
	//�Ǐ]
	bool isFollow_ = false;
	//�Ǐ]�p
	float followLengthMax_ = 0;
	//����
	float followT_ = 0;
	//�O�̍��W
	Vec2 oldPos_;

	//�Y�[��
	bool isZoom_ = false;
	float startZoomP_ = 0;
	float endZoomP_ = 0;
	int32_t zoomTimerMax_ = 0;
	int32_t zoomTimer_ = 0;

public:
	void Initialize();
	void Update();

private:
	void VisiableUpdate();
	void MatrixUpdate();
	void FollowUpdate();
	void ZoomUpdate();

public:
	void SetPos(const Vec2& pos) { pos_ = pos; }
	void SetZoom(const Vec2& zoom) { zoom_ = zoom; }
	void SetVisiableArea(const Vec4& visiableArea) { visiableArea_ = visiableArea; }
	void SetRot(float rot) { rot_ = rot; }

	const Vec2& GetPos() { return pos_; }
	const Vec2& GetZoom() { return zoom_; }
	const Vec4& GetVisiableArea() { return visiableArea_; }
	float GetRot() { return rot_; }
	float GetIsFollow() { return isFollow_; }
	const DirectX::XMMATRIX& GetCameraMatrix() { return cameraMatrix_; }

public:
	void BeginShake(uint32_t time, float maxLength);
	void BeginFollow(float maxLength);
	void EndFollow();
	void BeginZoom(float endZoomPow, int32_t time);
};