#pragma once
#include"SceneState.h"
#include"PostPera.h"
#include"ModelObj.h"
#include"Object.h"
#include"Player.h"


class SceneGame : public SceneState
{
private:
	const Vec3 PARTICLE_AMBIENT_ = { 0.1f,0.1f,0.0f };
	const Vec3 PARTICLE_DIFFUSE_ = { 10.1f,10.1f,0.1f };
	const Vec3 PARTICLE_SPECULAR_ = { 2240.8f,3240.8f,0.8f };

public:
	void Finalize()override;
	void Initialize()override;
	void Update()override;
	void DrawShadow()override;
	void Draw()override;
	void DrawSprite()override;

	void DrawImgui()override;
};

