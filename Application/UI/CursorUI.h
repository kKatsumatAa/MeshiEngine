#pragma once
#include"Sprite.h"


class CursorUI final
{
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
	CursorUI() { ; }
	~CursorUI() { ; }

public:
	CursorUI& operator=(const CursorUI& inst) = delete;
	CursorUI(const CursorUI& inst) = delete;

	static CursorUI& GetInstance();

public:
	void Initialize();
	void Update();
	void Draw();

public:
	const Vec2& GetCursorPos() { return cursorPos_; }
};