/*
* @file CursorUI.h
* @brief タイトルやセレクトにあるカーソル
*/

#pragma once
#include"Sprite.h"


class CursorUI final
{
public:
	const int8_t FLASHING_INTERVAL_ = 60;
	const Vec3 SPRITE_COLOR_ = { 8.0f,0.2f,0.2f };
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };

private:
	int32_t timer_ = 0;
	float alpha_ = 1.0f;
	Sprite texSprite_;
	Vec2 cursorPos_;
	Vec2 drawPos_;

	//点滅の回数（一秒間）
	const int32_t flashingNum_ = 2;
	//点滅の長さ
	const int32_t flashingIntervalWidth_ = 2;

public:
	//カーソルのサイズ
	static const  Vec2 cursorSize_;

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