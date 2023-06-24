#pragma once
#include"JsonLevelLoader.h"
#include"Object.h"
#include"Camera.h"

//jsonで読み込んだレベルを管理する
class LevelManager final
{
private://変数
	//Objectとモデルポインタのセット
	std::map<std::unique_ptr<Object>, Model*> objAndModels_;


private://関数
	LevelManager() { ; }
	~LevelManager();

public:
	//コピーコンストラクタ禁止
	LevelManager(const LevelManager& other) = delete;
	LevelManager& operator=(const LevelManager& other) = delete;

	static LevelManager& GetInstance();

	void StaticInitialize();
	//JsonLevelLoaderで読み込んだ内容を実際にエンジン上で読み込む
	void LoadLevelData();

	//読み込んだものを動かす
	void Update();
	//読み込んだものを描画
	void Draw(Camera* camera = nullptr);

};