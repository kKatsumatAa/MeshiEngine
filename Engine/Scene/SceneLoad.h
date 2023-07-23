#pragma once
#include"SceneState.h"
#include"Object.h"
#include"Async.h"


class SceneLoad : public SceneState
{
private:

public:
	void Load();

	void Finalize()override;
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;

	void DrawImgui()override;
};
