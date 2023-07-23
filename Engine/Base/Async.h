#pragma once
#include<thread>
#include<mutex>
#include<functional>
#include <future>


class Async
{
private:
	bool isLoading_ = false;
	std::mutex isLoadedMutex_;
	std::future<void> loadFunc_;


private:
	//���������Ă����֐�
	void AsyncLoad(std::function<void()> p);

public:
	//�񓯊����������Ă��邩
	bool GetIsLoading();
	void SetIsLoading(bool isLoading) { isLoading_ = isLoading; }

	//�񓯊����I������u�Ԃ�
	bool GetFinishedAsync();

	//�񓯊����J�n
	void StartAsyncFunction(const std::function<void()>& p);

	//lockFlag�����������΂Ɏ��s���邱��
	void EndThread();
};

