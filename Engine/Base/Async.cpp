#include "Async.h"

bool Async::GetIsLoading()
{
	return isLoading_;
}

void Async::AsyncLoad(std::function<void()> p)
{
	//関数
	p();
}

bool Async::GetFinishedAsync()
{
	if (isLoading_)
	{
		std::future_status status = loadFunc_.wait_for(std::chrono::seconds(0));
		//終わっていたら
		if (status == std::future_status::ready)
		{
			return true;
		}
	}
	return false;
}

void Async::StartAsyncFunction(const std::function<void()>& p)
{
	isLoading_ = true;
	//非同期処理スタート
	loadFunc_ = std::async(std::launch::async, [=] { return AsyncLoad(p); });
}

void Async::EndThread()
{
	//終わっていたら
	if (isLoading_)
	{
		isLoading_ = false;
	}
}
