/*
* @file SceneGame.h
* @brief ゲームシーンの処理
*/

#pragma once
#include"SceneState.h"
#include"PostPera.h"
#include"ModelObj.h"
#include"Object.h"
#include"Player.h"
#include "Replay.h"


class SceneGame : public SceneState
{
private:
	const Vec3 PARTICLE_AMBIENT_ = { 0.1f,0.1f,0.0f };
	const Vec3 PARTICLE_DIFFUSE_ = { 10.1f,10.1f,0.1f };
	const Vec3 PARTICLE_SPECULAR_ = { 2240.8f,3240.8f,0.8f };

	std::unique_ptr<Replay>replay_ = nullptr;

public:
	//終了処理
	void Finalize()override;
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
	//影用の深度描画
	void DrawShadow()override;
	//描画
	void Draw()override;
	//スプライト描画
	void DrawSprite()override;
	//ImGui描画
	void DrawImgui()override;
};

