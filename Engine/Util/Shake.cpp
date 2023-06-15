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
	shakeLength = length;
}

void Shake::SetLifeTime(int32_t time)
{
	lifeTimer = time;
	lifeTimerTmp = time;
}

void Shake::SetShake(int32_t time, float length)
{

	if (time > lifeTimer)
	{
		SetLifeTime(time);
		SetRandomLength(length);
	}
}

float Shake::GetShake()
{
	return  GetRandomShakeInternal(-shakeLength * t, shakeLength * t);
}

bool Shake::GetIsShaking()
{
	if (lifeTimer > 0)
	{
		return true;
	}

	return false;
}

void Shake::Initialize()
{
	shakeLength = 0.0f;
	lifeTimer = 0;
	lifeTimerTmp = 0;
	t = 0.0f;

}

void Shake::Update()
{
	if (lifeTimer != 0 && lifeTimerTmp != 0)
	{
		lifeTimer--;

		t = (float)lifeTimer / (float)lifeTimerTmp;
	}
}
