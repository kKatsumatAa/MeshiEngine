#pragma once
#include"Object.h"


class CursorUI final
{
private:
	int32_t timer_ = 0;
	const Vec2 cursorSizeExtend_ = { 0.01f,0.05f };
	Vec2 cursorSize_;
	float alpha_ = 1.0f;
	Object texObj_;
	Vec2 drawPos_;

	//�_�ł̉񐔁i��b�ԁj
	const int32_t flashingNum_ = 2;
	//�_�ł̒���
	const int32_t flashingIntervalWidth_ = 2;

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
};