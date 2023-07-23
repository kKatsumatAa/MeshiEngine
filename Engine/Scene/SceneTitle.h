#pragma once
#include"SceneState.h"
#include"Object.h"

class SceneTitle : public SceneState
{
private:
	int32_t titleTime_ = 0;
	const int32_t TITLE_TIME_MAX_ = 180;
	float t_ = 0;
	uint64_t titleTex_ = NULL;
	float titleScale_ = 0;
	const float TITLE_SCALE_MAX_ = 2.5f;
	Object titleObj_;



public:
	void Finalize()override;
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;

	void DrawImgui()override;
};