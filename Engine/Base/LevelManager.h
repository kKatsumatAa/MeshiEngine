#pragma once
#include"JsonLevelLoader.h"
#include"Object.h"
#include"Camera.h"
#include"Gun.h"

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
	void LoadLevelData(std::string fileName);

	//ファイル名によってクラス生成
	void LoadCharacter( LevelData::ObjectData& objData);

	//読み込んだものを動かす
	void Update();
	//読み込んだものを描画
	void Draw(Camera* camera = nullptr);

private://ゲームオリジナル
	Gun* GetChildGun(const LevelData::ObjectData& objData);
public:
	bool GetGameOver();
	bool GetGameClear();
};