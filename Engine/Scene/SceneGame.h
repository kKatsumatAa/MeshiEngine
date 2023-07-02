#pragma once
#include"SceneState.h"
#include"PostPera.h"
#include"Model.h"
#include"Object.h"
#include"Player.h"


class SceneGame : public SceneState
{
private:
	int32_t clearCoolTime_ = 0;
	const int32_t CLEAR_COOL_TIME_MAX_ = 20;

public:
	void Finalize()override;
	void Initialize()override;
	void Update(PostPera* postPera)override;
	void Draw()override;
	void DrawSprite()override;

	void DrawImgui()override;

	void DrawPostEffect()override;
	void DrawPostEffect2()override;
};

