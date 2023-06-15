#pragma once
#include<thread>
#include<mutex>
#include<functional>


class Async
{
private:
	bool isLoaded_ = false;
	std::mutex isLoadedMutex_;
	std::thread th1_;


private:
	//���������Ă����֐�
	void AsyncLoad(std::function<void()> p);


public:
	//�񓯊��������I��������ǂ���
	bool GetLockFlag();

	//�t���O���Z�b�g
	void SetLockFlag(bool islock);

	//�񓯊����J�n
	void StartAsyncFunction(const std::function<void()>& p);

	//lockFlag�����������΂Ɏ��s���邱��
	void EndThread();
};

