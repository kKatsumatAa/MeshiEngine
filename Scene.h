#pragma once
#include "Enemy.h"
#include "CollisionManager.h"
#include"EnemyManager.h"
#include"Background.h"


class Scene;

class SceneState
{
protected:
	Scene* scene;
	Draw title;

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
	ViewMat viewMat;
	ProjectionMat projectionMat;
	//UI
	Draw UI;
	//パーティクル
	ParticleManager pManager;
	//BulletManager
	BulletManager bulletManager;
	//player
	Player* player = new Player();
	//item
	ItemManager iManager;
	//enemy
	EnemyManager enemyManager;
	//衝突
	std::unique_ptr<CollisionManager> colliderManager = std::make_unique<CollisionManager>(&viewMat, &projectionMat);
	//背景
	Background back;
	
	int bossNum = 0;

	int hitStopTimer = 0;
	const int hitStoptmp = 5;


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
	Vec3 pos = { 0,0,0 };
	float count = 0;

public:
	void Initialize()override;
	void Update(SoundData* soundData)override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle)override;
};

class SceneGame : public SceneState
{
private:
	bool isHit[2] = { 0,0 };

public:
	void Initialize()override;
	void Update(SoundData* soundData)override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle)override;
};

class SceneEnd : public SceneState
{
public:
	void Initialize()override;
	void Update(SoundData* soundData) override;
	void Draw(UINT64* textureHandle, UINT64* textureNumHundle) override;

};
