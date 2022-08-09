#pragma once
#include<functional>

/// <summary>
/// 時限発動クラス
/// </summary>
class TimedCall
{
public:
	TimedCall(std::function<void()> f, uint32_t time);

	void Update();
	bool IsFinished() { return isFinish; }

private:
	//コールバック
	std::function<void()> f;
	//残り時間
	uint32_t time_;
	//完了フラグ
	bool isFinish = false;

};

