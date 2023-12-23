#include "Shake.h"

float Shake::GetRandom(float min, float max)
{
	std::uniform_real_distribution<float> random(min, max);
	//乱数生成器
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	return random(engine);
}

void Shake::SetShakeLength(float length)
{
	shakeLength_ = length;
}

void Shake::SetLifeTime(int32_t time)
{
	lifeTimer_ = time;
	lifeTimerTmp_ = time;
}

void Shake::BeginShake(int32_t time, float length)
{
	//新たにシェイク始める時、今のシェイクより長い時間なら
	if (time > lifeTimer_)
	{
		SetLifeTime(time);
		SetShakeLength(length);
	}
}

float Shake::GetShake()
{
	return  GetRandom(-shakeLength_ * t_, shakeLength_ * t_);
}

bool Shake::GetIsShaking()
{
	if (lifeTimer_ > 0)
	{
		return true;
	}

	return false;
}

void Shake::Initialize()
{
	shakeLength_ = 0.0f;
	lifeTimer_ = 0;
	lifeTimerTmp_ = 0;
	t_ = 0.0f;

}

void Shake::Update()
{
	if (lifeTimer_ != 0 && lifeTimerTmp_ != 0)
	{
		lifeTimer_--;

		t_ = (float)lifeTimer_ / (float)lifeTimerTmp_;
	}
}
