#pragma once
#include"SceneState.h"
#include"Object.h"
#include"Async.h"


class SceneLoad : public SceneState
{
private:
	int32_t count_ = 0;
	Async async_;
	Object loadObj_;

public:
	void Load();

	void Finalize()override;
	void Initialize()override;
	void Update(PostPera* postPera) override;
	void Draw() override;
	void DrawSprite()override;

	void DrawImgui()override;

	void DrawPostEffect()override;
	void DrawPostEffect2()override;
};
