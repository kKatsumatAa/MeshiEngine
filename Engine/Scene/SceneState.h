#pragma once
#include"PostPera.h"

class SceneManager;

class SceneState
{
protected:
	//�|�C���^�؂�Ă邾���Ȃ̂�delete������~
	SceneManager* sceneM_;

public:
	virtual ~SceneState() { ; }

	virtual void Finalize() = 0;
	virtual void Initialize() = 0;

	void SetSceneManager(SceneManager* sceneM);
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawSprite() = 0;

	virtual void DrawImgui() = 0;
};

