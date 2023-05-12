#pragma once
#include"SceneState.h"
#include"PostPera.h"


class SceneGame : public SceneState
{
private:
	PostPera postPera;

public:
	void Finalize()override;
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;

	void DrawImgui()override;

	void DrawPostEffect()override;
	void DrawPostEffect2()override;
};

