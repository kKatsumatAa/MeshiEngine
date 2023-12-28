/*
* @file TitleUI.h
* @brief タイトルのUI
*/

#pragma once
#include"Sprite.h"


class TitleUI final
{
public:
	const Vec4 SPRITE_COLOR_ = { 3.0f,3.0f,3.0f,1.0f };
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };

private:
	float t_ = 0;
	int32_t timer_ = 0;
	const int32_t TITLE_TIME_MAX_ = 180;
	uint64_t titleTex_ = NULL;
	float titleScale_ = 0;
	const float TITLE_SCALE_MAX_ = 2.5f;
	Sprite titleSprite_;


private:
	//コンストラクタ
	TitleUI() { ; }
	//デストラクタ
	~TitleUI() { ; }

public:
	//コピーコンストラクタ禁止
	TitleUI& operator=(const TitleUI& inst) = delete;
	//コピーコンストラクタ禁止
	TitleUI(const TitleUI& inst) = delete;

	//インスタンス取得
	static TitleUI& GetInstance();

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//スプライト描画
	void DrawSprite();
};