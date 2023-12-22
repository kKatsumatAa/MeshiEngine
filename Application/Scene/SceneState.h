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
	virtual ~SceneState() { ; }

	virtual void Finalize() = 0;
	virtual void Initialize() = 0;

	void SetSceneManager(SceneManager* sceneM);
	virtual void Update() = 0;
	virtual void DrawShadow() = 0;
	virtual void Draw() = 0;
	virtual void DrawSprite() = 0;

	virtual void DrawImgui() = 0;
};

