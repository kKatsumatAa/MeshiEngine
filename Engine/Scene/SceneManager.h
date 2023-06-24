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
#include "LevelManager.h"



class CollisionManager;
class Player;

class SceneManager final
{
private:
	//シーンの状態（行動）
	std::unique_ptr<SceneState> state_;
	//シーンファクトリー（ポインタを借りる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

public:
	Model* model_[20];

	Object draw_[10];

	ModelFBX* modelFBX_;

	DebugText debugText_;

	//デバッグテキスト
	uint64_t debugTextHandle_;
	uint64_t texhandle_[10];

	//数字の画像
	uint64_t textureNumHundle_[12];

	//ライト
	std::unique_ptr<LightManager> lightManager_;

	float ambientColor_[3] = { 1,1,1 };
	float diffuseColor_[3] = { 1,1,1 };
	float specularColor_[3] = { 1,1,1 };

	//丸影
	float circleShadowDir_[3] = { 0,-1,0 };
	float circleShadowAtten_[3] = { 0.5f,0.15f,0 };
	float circleShadowFactorAngle_[2] = { 0,5.0f };
	float circleShadowDistance_ = 100.0f;

	float fighterPos_[3] = { -10.0f,0,0 };


	std::unique_ptr<Camera> camera_;



private:
	~SceneManager();
	SceneManager() { ; }

public:
	static SceneManager& GetInstance() { static SceneManager sInst; return sInst; }
	//コピーコンストラクタを無効
	SceneManager(const SceneManager& obj) = delete;
	//代入演算子も
	SceneManager& operator=(const SceneManager& obj) = delete;

public:
	//シーンステート変更
	void ChangeScene(std::string sceneName);

	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	void Initialize();
	void Update(PostPera* postPera);
	void Draw();
	void DrawPostEffect();
	void DrawPostEffect2();
	void DrawSprite();

	void DrawImgui();

	void StopWaveAllScene();
};