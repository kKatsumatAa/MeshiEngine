#include "Async.h"

bool Async::GetIsLoading()
{
	return isLoading_;
}

void Async::AsyncLoad(std::function<void()> p)
{
	//�֐�
	p();
}

bool Async::GetFinishedAsync()
{
	if (isLoading_)
	{
		std::future_status status = loadFunc_.wait_for(std::chrono::seconds(0));
		//�I����Ă�����
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
	//�񓯊������X�^�[�g
	loadFunc_ = std::async(std::launch::async, [=] { return AsyncLoad(p); });
}

void Async::EndThread()
{
	//�I����Ă�����
	if (isLoading_)
	{
		isLoading_ = false;
	}
}
