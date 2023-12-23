/*
* @file SceneStageSelect.h
* @brief ステージセレクトのシーン
*/

#pragma once
#include"SceneState.h"
#include"Object.h"

class SceneStageSelect : public SceneState
{
private:

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
	//ImGuiの描画
	void DrawImgui()override;
};