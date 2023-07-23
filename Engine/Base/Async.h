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
	//処理をしてくれる関数
	void AsyncLoad(std::function<void()> p);

public:
	//非同期処理をしているか
	bool GetIsLoading();
	void SetIsLoading(bool isLoading) { isLoading_ = isLoading; }

	//非同期が終わった瞬間か
	bool GetFinishedAsync();

	//非同期を開始
	void StartAsyncFunction(const std::function<void()>& p);

	//lockFlagがたったら絶対に実行すること
	void EndThread();
};

