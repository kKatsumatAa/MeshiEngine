#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"
#include"ColliderManager.h"
#include"ParticleManager.h"
#include"Camera.h"
#include"Async.h"



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

	//当たり判定レイ
	Ray ray;

	std::unique_ptr<ColliderManager> colliderM;

	std::unique_ptr<Camera> camera;

public:
	~Scene();
	void ChangeState(SceneState* state);

	void Initialize();
	void Update();
	void Draw();
	void DrawPostEffect();
	void DrawSprite();

	void StopWaveAllScene();
};

class SceneTitle : public SceneState
{
private:


public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;
};

class SceneGame : public SceneState
{
private:


public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
	void DrawSprite()override;
};

class SceneGameOver : public SceneState
{
private:
	int count = 0;
	const int countTmp = 150;

public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class SceneClear : public SceneState
{
private:
	int count = 0;
	const int countTmp = 150;


public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};

class SceneLoad : public SceneState
{
private:
	int count = 0;
	Async async;

public:
	void StageCreate();

	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};
