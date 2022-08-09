#include "TimedCall.h"

TimedCall::TimedCall(std::function<void()> f, uint32_t time) :
	f(f), time_(time)
{
}

void TimedCall::Update()
{
	if (IsFinished())
	{
		return;
	}

	time_--;
	if (time_ <= 0)
	{
		isFinish = true;
		//コールバック関数呼び出し
		f();
	}
}
