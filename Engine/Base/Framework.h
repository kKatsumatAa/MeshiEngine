#pragma once
#include"SceneManager.h"
#include"DebugText.h"
#include"FbxLoader.h"
#include"AbstractSceneFactory.h"
#include"CameraManager.h"
#include"MouseInput.h"
#include"PostEffectManager.h"


//ゲーム全体（共通処理）
class Framework
{
protected:
	SceneManager* sceneM_ = nullptr;

	MSG msg_{};	//メッセージ

	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	std::unique_ptr <ImGuiManager> imguiM_;

	bool isEndGame_ = false;

public:
	//仮想デストラクタ(派生クラスのデストラクタを呼べるように)
	virtual ~Framework() = default;

	//初期化
	virtual void Initialize();

	//終了
	virtual void Finalize();

	//毎フレーム更新
	virtual void Update();

	//描画(継承しないとインスタンスを生成出来なくするため)
	virtual void Draw() = 0;

public:
	//実行
	void Run();
};

