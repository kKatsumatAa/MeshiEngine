#pragma once

class SceneManager;

class SceneState
{
protected:
	//ポインタ借りてるだけなのでdeleteしたら×
	SceneManager* sceneM_;

public:
	virtual void Finalize() = 0;
	virtual void Initialize() = 0;

	void SetScene(SceneManager* sceneM);
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawSprite() = 0;

	virtual void DrawImgui() = 0;

	virtual void DrawPostEffect() = 0;
	virtual void DrawPostEffect2() = 0;
};

