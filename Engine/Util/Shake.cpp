#include "Shake.h"

float Shake::GetRandomShakeInternal(float min, float max)
{
	std::uniform_real_distribution<float> random(min, max);
	//—”¶¬Ší
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	return random(engine);
}

void Shake::SetRandomLength(float length)
{
	shakeLength_ = length;
}

void Shake::SetLifeTime(int32_t time)
{
	lifeTimer_ = time;
	lifeTimerTmp_ = time;
}

void Shake::SetShake(int32_t time, float length)
{

	if (time > lifeTimer_)
	{
		SetLifeTime(time);
		SetRandomLength(length);
	}
}

float Shake::GetShake()
{
	return  GetRandomShakeInternal(-shakeLength_ * t_, shakeLength_ * t_);
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
