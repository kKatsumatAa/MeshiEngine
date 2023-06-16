#pragma once
#include"Util.h"
#include<random>

//乱数シード生成器
static std::random_device sSeedGen;
//メルセンヌツイスター
static std::mt19937_64 sEngine(sSeedGen());

class Shake
{
private:
	float shakeLength_ = 0.0f;
	int32_t   lifeTimer_ = 0;
	int32_t   lifeTimerTmp_ = 0;
	float t_ = 0.0f;

private:
	float GetRandomShakeInternal(float min, float max);
	void SetRandomLength(float length);
	void SetLifeTime(int32_t time);

public:
	void SetShake(int32_t time, float length);
	float GetShake();

	bool GetIsShaking();

	void Initialize();

	void Update();

};

