#pragma once
#include"SceneState.h"
#include"Object.h"

class SceneTitle : public SceneState
{
private:

public:
	void Finalize()override;
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;

	void DrawImgui()override;
};