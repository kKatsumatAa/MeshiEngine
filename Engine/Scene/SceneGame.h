#pragma once
#include"SceneState.h"
#include"PostPera.h"
#include"ModelObj.h"
#include"Object.h"
#include"Player.h"


class SceneGame : public SceneState
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

