#include "Wave.h"
#include "Util.h"

void Wave::BeginWave(const Vec3& epicenter, const Vec2& thickness, float distanceMax, float timer)
{
	timer_ = 0;
	timerMax_ = timer;

	epicenter_ = epicenter;
	thicknessMax_ = thickness;
	distanceMax_ = distanceMax;

	Update();
}

void Wave::Update()
{
	float t = min(timer_ / timerMax_, 1.0f);

	thickness_ = LerpVec2(thicknessMax_, { 0,0 }, EaseOut(t));
	distance_ = Lerp(0, distanceMax_, EaseOut(t));

	timer_++;
}

void Wave::End()
{
	BeginWave({ 0,0,0 }, { 0,0 }, 0, 0);
}

bool Wave::GetIsEndWave()
{
	if (timer_ >= timerMax_)
	{
		return true;
	}

	return false;
}
