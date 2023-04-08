#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"
#include"ParticleManager.h"
#include"Camera.h"
#include"Async.h"
#include "PadInput.h"

class CollisionManager;
class Player;


class Scene;

class SceneState
{
protected:
	Scene* scene;

public:
	virtual void Initialize() = 0;
	void SetScene(Scene* scene);
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawSprite() = 0;
};

class Scene
{
private:
	//状態（行動）
	SceneState* state = nullptr;

public:
	WorldMat cameraWorldMat;
	const Vec3 cameraPos = { 0,0,-100 };

	Model* model[20];

	Object draw[10];

	ModelFBX* modelFBX;

	DebugText debugText;

	//デバッグテキスト
	UINT64 debugTextHandle;
	UINT64 texhandle[10];

	//画像用ハンドル
	UINT64 textureHandle[30] = { 0 };

	//数字の画像
	UINT64 textureNumHundle[12];


	//imgui
	ImGuiManager* imGuiManager;

	//ライト
	LightManager* lightManager = nullptr;

	//丸影
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.15f,0 };
	float circleShadowFactorAngle[2] = { 0,5.0f };
	float circleShadowDistance = 100.0f;

	float fighterPos[3] = { 1.0f,0,0 };


	std::unique_ptr<Camera> camera;



public:
	~Scene();
	void ChangeState(SceneState* state);

	void Initialize();
	void Update();
	void Draw();
	void DrawPostEffect();
	void DrawPostEffect2();
	void DrawSprite();

	void StopWaveAllScene();
};

class SceneLoad : public SceneState
{
private:
	int count = 0;
	Async async;
	Object loadObj;

public:
	void Load();

	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class SceneBasic : public SceneState
{
private:


public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;
};

class Scene1 : public SceneState
{
private:


public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;
};

class Scene2 : public SceneState
{
private:
	float pointLightPos[3] = { 0,0,-10 };
	float pointLightColor[3] = { 1.0f,1.0f,1.0f };
	float pointLightAtten[3] = { 0.01f,0.01f,0.01f };


public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class Scene3 : public SceneState
{
private:
	float spotLightDir[3] = { 0,-1,0 };
	float spotLightPos[3] = { 0,30,0 };
	float spotLightColor[3] = { 1.0f,1.0f,1.0f };
	float spotLightAtten[3] = { 0.001f,0.001f,0.001f };
	float spotLightFactorAngle[2] = { 10.0f,30.0f };

public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class Scene4 : public SceneState
{
private:
	//丸影
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.15f,0 };
	float circleShadowFactorAngle[2] = { 0,5.0f };
	float circleShadowDistance = 100.0f;

	float fighterPos[3] = { 1.0f,0,0 };

public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class Scene5 : public SceneState
{
private:
	//コライダー
	Player* objPlayer;
	CollisionManager* collisionManager;
	Object obj;

public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class Scene6 : public SceneState
{
private:
	int count = 0;
	const int countMax = 60;

public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};
