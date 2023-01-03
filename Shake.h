#pragma once
#include"Util.h"

class Shake
{
private:
	float shakeLength = 0.0f;
	int   lifeTimer = 0;
	int   lifeTimerTmp = 0;
	float t = 0.0f;

private:
	float GetRandomShakeInternal(float min, float max);
	void SetRandomLength(float length);
	void SetLifeTime(int time);

public:
	void SetShake(int time, float length);
	float GetShake();

	void Initialize();

	void Update();

};

