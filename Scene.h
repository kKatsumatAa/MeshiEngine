#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"


class Scene;

class SceneState
{
protected:
	Scene* scene;

public:
	virtual void Initialize() = 0;
	void SetScene(Scene* scene);
	virtual void Update(SoundData* soundData) = 0;
	virtual void Draw(UINT64* textureHandle, UINT64* textureNumHundle) = 0;
};

class Scene
{
private:
	//状態（行動）
	SceneState* state = nullptr;

public:
	SoundData* soundData;
	WorldMat cameraWorldMat;
	const Vec3 cameraPos = { 0,0,-100 };
	ViewMat viewMat;
	ProjectionMat projectionMat;

	Model* model[5];

	Draw draw[10];
	

	Sphere tama[2];
	Plane plane;
	Triangle triangle;
	DebugText debugText;

	//デバッグテキスト
	UINT64 debugTextHandle;
	UINT64 texhandle[10];

	//imgui
	ImGuiManager* imGuiManager;

public:
	~Scene();
	void ChangeState(SceneState* state);

	void Initialize(SoundData* soundData);
	void Update(SoundData* soundData);
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle);
};

class SceneTitle : public SceneState
{
private:


public:
	void Initialize()override;
	void Update(SoundData* soundData)override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle)override;
};

class SceneGame : public SceneState
{
private:


public:
	void Initialize()override;
	void Update(SoundData* soundData)override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle)override;
};

class SceneEnd : public SceneState
{
private:


public:
	void Initialize()override;
	void Update(SoundData* soundData) override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle) override;

};
