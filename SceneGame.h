#pragma once
#include"SceneState.h"


class SceneGame : public SceneState
{
private:


public:
	void Finalize()override;
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;
};

