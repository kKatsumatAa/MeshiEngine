/*
* @file TitleUI.h
* @brief タイトルのUI
*/

#pragma once
#include"Sprite.h"


class TitleUI final
{
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