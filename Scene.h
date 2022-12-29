#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"
#include"EnemyManager.h"
#include"PlayerBulletManager.h"
#include"ColliderManager.h"
#include"Stage.h"
#include"ItemManager.h"
#include"ParticleManager.h"


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
	ViewMat viewMat;
	ProjectionMat projectionMat;

	Model* model[5];

	Object draw[10];


	Sphere tama[2];
	Plane plane;
	Triangle triangle;
	DebugText debugText;

	//デバッグテキスト
	UINT64 debugTextHandle;
	UINT64 texhandle[10];

	//画像用ハンドル
	UINT64 textureHandle[30] = { 0 };

	//数字の画像
	UINT64 textureNumHundle[12];

	//音データ
	Sound::SoundData soundData[10];

	//imgui
	ImGuiManager* imGuiManager;

	float pos[2] = { 100,100 };
	float rot = 0;
	float scale = 1.0f;
	float uvwidth = 0;
	float color = 0;//

	//ライト
	LightManager* lightManager = nullptr;

	float pointLightPos[3] = { 0,0,0 };
	float pointLightColor[3] = { 1.0f,1.0f,1.0f };
	float pointLightAtten[3] = { 0.3f,0.1f,0.1f };

	//player
	std::unique_ptr<Player> player;

	std::unique_ptr<EnemyManager> enemyM;

	std::unique_ptr<ColliderManager> colliderM;

	std::unique_ptr<PlayerBulletManager> playerBulletM;

	std::unique_ptr<Stage> stage;

	std::unique_ptr<ItemManager> itemM;

public:
	~Scene();
	void ChangeState(SceneState* state);

	void Initialize();
	void Update();
	void Draw();
	void DrawSprite();
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

class SceneEnd : public SceneState
{
private:


public:
	void Initialize()override;
	void Update() override;
	void Draw() override;
	void DrawSprite()override;
};
