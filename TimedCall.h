#pragma once
#include<functional>

/// <summary>
/// ���������N���X
/// </summary>
class TimedCall
{
public:
	TimedCall(std::function<void()> f, uint32_t time);

	void Update();
	bool IsFinished() { return isFinish; }

private:
	//�R�[���o�b�N
	std::function<void()> f;
	//�c�莞��
	uint32_t time_;
	//�����t���O
	bool isFinish = false;

};

