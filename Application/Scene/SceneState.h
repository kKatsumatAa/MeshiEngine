/*
* @file SceneState.h
* @brief シーンの状態基底クラス
*/

#pragma once
#include"PostPera.h"

//シーンマネージャーの前方宣言
class SceneManager;

class SceneState
{
protected:
	//ポインタ借りてるだけなのでdeleteしたら×
	SceneManager* sceneM_;

public:
	//デストラクタ
	virtual ~SceneState() { ; }

	//終了処理
	virtual void Finalize() = 0;
	//初期化
	virtual void Initialize() = 0;

public:
	//シーンマネージャーのポインタセット
	void SetSceneManager(SceneManager* sceneM);

	//更新
	virtual void Update() = 0;
	//影用の深度描画
	virtual void DrawShadow() = 0;
	//描画
	virtual void Draw() = 0;
	//スプライト描画
	virtual void DrawSprite() = 0;
	//ImGuiの描画
	virtual void DrawImgui() = 0;
};

