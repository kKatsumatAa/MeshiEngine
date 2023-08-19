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
#include "MouseInput.h"
#include "PostEffectManager.h"



class CollisionManager;
class Player;

class SceneManager final
{
private:
	//シーンの状態（行動）
	std::unique_ptr<SceneState> state_;
	std::unique_ptr<SceneState> nextScene_;
	//シーンファクトリー（ポインタを借りる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

private:
	uint64_t texhandle_[10];

	//数字の画像
	uint64_t textureNumHundle_[12];

	//ライト
	std::unique_ptr<LightManager> lightManager_;

	float ambientColor_[3] = { 1,1,1 };
	float diffuseColor_[3] = { 1,1,1 };
	float specularColor_[3] = { 1,1,1 };

	std::unique_ptr<Camera> camera_;

	//ゲームが終了したか
	bool isEscapingGame_ = false;

private:
	~SceneManager();
	SceneManager() { ; }

public:
	static SceneManager& GetInstance() { static SceneManager sInst; return sInst; }
	//コピーコンストラクタを無効
	SceneManager(const SceneManager& obj) = delete;
	//代入演算子も
	SceneManager& operator=(const SceneManager& obj) = delete;

	//シーンステート変更
	void ChangeScene();

public:
	//次のシーンセット
	void SetNextScene(std::string sceneName);

	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	void Initialize();
	void Update();
	void Draw();
	void DrawSprite();

	void DrawImgui();

	void StopWaveAllScene();

public:
	void SetIsEscapingGame(bool is) { isEscapingGame_ = is; }
	bool GetIsEscapingGame() { return isEscapingGame_; }

};