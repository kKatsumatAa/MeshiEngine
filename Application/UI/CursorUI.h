/*
* @file CursorUI.h
* @brief タイトルやセレクトにあるカーソル
*/

#pragma once
#include"Sprite.h"


class CursorUI final
{
public:
	//カーソルのサイズ
	static const  Vec2 cursorSize_;
public:
	const int8_t FLASHING_INTERVAL_ = 60;
	const Vec3 SPRITE_COLOR_ = { 8.0f,0.2f,0.2f };
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };
	const float SPRITE_NORMAL_SCALE_ = 1.0f;
	const float CLAMP_CURSOR_POS_SIZE_RATE_ = 2.0f;
	const float DRAW_POS_ADD_SIZE_RATE_ = 0.5f;
	const float NORMAL_ALPHA_ = 1.0f;
	//点滅の回数（一秒間）
	const int32_t flashingNum_ = 2;
	//点滅の長さ
	const int32_t flashingIntervalWidth_ = 2;

private:
	int32_t timer_ = 0;
	float alpha_ = 1.0f;
	Sprite texSprite_;
	Vec2 cursorPos_;
	Vec2 drawPos_;


private:
	//コンストラクタ
	CursorUI() { ; }
	//デストラクタ
	~CursorUI() { ; }

public:
	//コピーコンストラクタ禁止
	CursorUI& operator=(const CursorUI& inst) = delete;
	//コピーコンストラクタ禁止
	CursorUI(const CursorUI& inst) = delete;

	//インスタンス取得
	static CursorUI& GetInstance();

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

public:
	//カーソル位置取得
	const Vec2& GetCursorPos() { return cursorPos_; }
};