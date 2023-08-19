#pragma once
#include"Object.h"


class CursorUI final
{
private:
	int32_t timer_ = 0;
	float alpha_ = 1.0f;
	Object texObj_;
	Vec2 cursorPos_;
	Vec2 drawPos_;

	//�_�ł̉񐔁i��b�ԁj
	const int32_t flashingNum_ = 2;
	//�_�ł̒���
	const int32_t flashingIntervalWidth_ = 2;

public:
	//�J�[�\���̃T�C�Y
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