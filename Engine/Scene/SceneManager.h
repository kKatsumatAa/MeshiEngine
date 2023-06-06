#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"
#include"ParticleManager.h"
#include"Camera.h"
#include"Async.h"
#include "PadInput.h"
#include "SceneState.h"
#include "AbstractSceneFactory.h"
#include "ModelManager.h"



class CollisionManager;
class Player;

class SceneManager final
{
private:
	//シーンの状態（行動）
	SceneState* state = nullptr;
	//シーンファクトリー（ポインタを借りる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

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

	//ライト
	LightManager* lightManager = nullptr;

	float ambientColor[3] = { 0,0,1 };
	float diffuseColor[3] = { 1,0,0 };
	float specularColor[3] = { 0,1,0 };

	//丸影
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.15f,0 };
	float circleShadowFactorAngle[2] = { 0,5.0f };
	float circleShadowDistance = 100.0f;

	float fighterPos[3] = { 1.0f,0,0 };


	std::unique_ptr<Camera> camera;



private:
	~SceneManager();
	SceneManager() { ; }

public:
	static SceneManager& GetInstance() { static SceneManager inst; return inst; }
	//コピーコンストラクタを無効
	SceneManager(const SceneManager& obj) = delete;
	//代入演算子も
	SceneManager& operator=(const SceneManager& obj) = delete;

public:
	//シーンステート変更
	void ChangeScene(std::string sceneName);

	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

	void Initialize();
	void Update();
	void Draw();
	void DrawPostEffect();
	void DrawPostEffect2();
	void DrawSprite();

	void DrawImgui();

	void StopWaveAllScene();
};