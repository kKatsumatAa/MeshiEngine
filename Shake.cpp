#include "Shake.h"
#include "Collider.h"

float Shake::GetRandomShakeInternal(float min, float max)
{
	std::uniform_real_distribution<float> random(min, max);
	return random(engine);
}

void Shake::SetRandomLength(float length)
{
	this->shakeLength = length;
}

void Shake::SetLifeTime(int time)
{
	this->lifeTimer = time;
	this->lifeTimerTmp = time;
}

void Shake::SetShake(int time, float length)
{
	if (time > this->lifeTimer)
	{
		SetLifeTime(time);
		SetRandomLength(length);
	}
}

float Shake::GetShake()
{
	return  GetRandomShakeInternal(-shakeLength * t, shakeLength * t);
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
