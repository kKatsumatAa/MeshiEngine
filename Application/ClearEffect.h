#pragma once
#include"Object.h"


class ClearEffect final
{
private:
	int32_t timer_ = 0;
	const int32_t TIMER_MAX_ = 260;
	float t_ = 0;

	float scale_ = 0;

	const float MAX_SCALE_ = 1.5f;

	Object obj_;

	uint64_t texHandle_ = NULL;

	float alpha_ = 0.0f;

private:
	ClearEffect() { ; }
	~ClearEffect() { ; }

public:
	ClearEffect& operator=(const ClearEffect& obj) = delete;
	ClearEffect(const ClearEffect& obj) = delete;

	static ClearEffect& GetInstance();

	void Initialize();
	void Update();
	void Draw();

};