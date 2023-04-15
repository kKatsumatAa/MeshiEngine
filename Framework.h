#pragma once
#include"SceneManager.h"
#include"DebugText.h"
#include"FbxLoader.h"


//ゲーム全体（共通処理）
class Framework
{
protected:
	SceneManager* sceneM = nullptr;

	MSG msg{};	//メッセージ


public:
	//仮想デストラクタ(派生クラスのデストラクタを呼べるように)
	virtual ~Framework() = default;

	//初期化
	virtual void Initialize();

	//終了
	virtual void Finalize();

	//毎フレーム更新
	virtual bool Update();

	//描画(継承しないとインスタンスを生成出来なくするため)
	virtual void Draw() = 0;

public:
	//実行
	void Run();
};

