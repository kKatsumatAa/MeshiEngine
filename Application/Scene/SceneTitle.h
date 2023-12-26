/*
* @file SceneTitle.h
* @brief タイトルシーン
*/

#pragma once
#include"SceneState.h"
#include"Object.h"

class SceneTitle : public SceneState
{
public:
	const float POST_BARREL_POW_ = 0.7f;
	const Vec4 POST_SCREEN_COLOR_ = { 1.0f,1.0f,1.0f,1.0f };
	const Vec3 POST_SEA_CAMERA_POS_ = { 0,43.0f,0 };
	const Vec3 POST_SEA_CAMERA_ROT_ = { -0.5f,0,0 };
	const Vec3 POST_SEA_COLOR_ = { 6.0f,0.6f,0.1f };
	const Vec3 POST_SKY_COLOR_ = { 0,0,0 };

public:
	//終了処理
	void Finalize()override;
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//影用の深度描画
	void DrawShadow()override;
	//描画
	void Draw()override;
	//スプライト描画
	void DrawSprite()override;
	//ImGuiの描画
	void DrawImgui()override;
};