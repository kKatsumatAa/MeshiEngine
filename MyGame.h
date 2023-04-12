#pragma once
#include"Scene.h"
#include"DebugText.h"
#include"FbxLoader.h"


//ゲーム全体
class MyGame
{
private:
	Scene* scene;

	MSG msg{};	//メッセージ

public:
	//初期化
	void Initialize();

	//終了
	void Finalize();

	//毎フレーム更新
	bool Update();

	//描画
	void Draw();
};

