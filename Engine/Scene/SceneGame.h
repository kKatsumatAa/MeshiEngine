#pragma once
#include"SceneState.h"
#include"PostPera.h"
#include"Model.h"
#include"Object.h"
#include"Player.h"


class SceneGame : public SceneState
{
private:
	std::unique_ptr<Player> player_;

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

