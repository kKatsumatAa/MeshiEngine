#pragma once
#include"SceneState.h"
#include"Object.h"
#include"Async.h"


class SceneLoad : public SceneState
{
private:
	int count = 0;
	Async async;
	Object loadObj;

public:
	void Load();

	void Finalize()override;
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};
