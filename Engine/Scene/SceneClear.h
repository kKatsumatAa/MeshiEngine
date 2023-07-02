#pragma once
#include"SceneState.h"


class SceneClear : public SceneState
{
private:

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