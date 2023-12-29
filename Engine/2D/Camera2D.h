/*
* @file Camera2D.h
* @brief 2d用のカメラ
*/

#pragma once
#include"ViewMat.h"
#include"ProjectionMat.h"
#include"Shake.h"

//2D用のカメラ
class Camera2D
{
public:
	const float VISIABLE_AREA_SUB_RATE_ = 0.5f;
	const float START_ZOOM_RATE_ = 0.5f;
	const float ZOOM_TIMER_RATIO_MAX_ = 1.0f;
	const float FOLLOW_RATIO_RATE_ = 1.0f / 9.0f;
	const float FOLLOW_RATIO_MAX_ = 1.0f;

private:
	//位置
	Vec2 pos_ = { WindowsApp::WINDOW_WIDTH_ / 2.0f,WindowsApp::WINDOW_HEIGHT_ / 2.0f };
	//拡大率
	Vec2 zoom_ = { 1.0f,1.0f };
	//見える範囲
	Vec4 visiableArea_ = { 0,0,WindowsApp::WINDOW_WIDTH_,WindowsApp::WINDOW_HEIGHT_ };
	//角度
	float rot_ = 0.0f;
	//スクリーン中心
	Vec2 screenCenterPos_ = { WindowsApp::WINDOW_WIDTH_ / 2.0f,WindowsApp::WINDOW_HEIGHT_ / 2.0f };
	//カメラ行列
	DirectX::XMMATRIX cameraMatrix_;
	//シェイク
	Shake shake_;

	//追従
	bool isFollow_ = false;
	//追従用
	float followLengthMax_ = 0;
	//割合
	float followT_ = 0;
	//前の座標
	Vec2 oldPos_;

	//ズーム
	bool isZoom_ = false;
	float startZoomP_ = 0;
	float endZoomP_ = 0;
	int32_t zoomTimerMax_ = 0;
	int32_t zoomTimer_ = 0;

public:
	//初期化
	void Initialize();
	//更新
	void Update();

private:
	//見える範囲の更新
	void VisiableUpdate();
	//行列更新
	void MatrixUpdate();
	//追尾更新
	void FollowUpdate();
	//拡大更新
	void ZoomUpdate();

public:
	//位置セット
	void SetPos(const Vec2& pos) { pos_ = pos; }
	//拡大セット
	void SetZoom(const Vec2& zoom) { zoom_ = zoom; }
	//見える範囲セット
	void SetVisiableArea(const Vec4& visiableArea) { visiableArea_ = visiableArea; }
	//角度セット
	void SetRot(float rot) { rot_ = rot; }

	//位置取得
	const Vec2& GetPos() { return pos_; }
	//拡大取得
	const Vec2& GetZoom() { return zoom_; }
	//見える範囲取得
	const Vec4& GetVisiableArea() { return visiableArea_; }
	//角度取得
	float GetRot() { return rot_; }
	//追尾してるか取得
	float GetIsFollow() { return isFollow_; }
	//カメラ行列取得
	const DirectX::XMMATRIX& GetCameraMatrix() { return cameraMatrix_; }

public:
	//シェイク開始
	void BeginShake(uint32_t time, float maxLength);
	//追尾開始
	void BeginFollow(float maxLength);
	//追尾終わり
	void EndFollow();
	//拡大開始
	void BeginZoom(float endZoomPow, int32_t time);
};