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

	//地形オブジェクトとして使うか
	bool isLandShape_ = false;


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
	void LoadObj(LevelData::ObjectData& objData);

	//読み込んだものを動かす
	void Update();
	//読み込んだものを描画
	void Draw(Camera* camera = nullptr);
	void DrawImGui();

private://ゲームオリジナル
	//武器のポインタ返す
	Weapon* GetChildWeapon(LevelData::ObjectData& objData);

	//地形オブジェクトとして使うかチェック
	void CheckLandShapeObject(const LevelData::ObjectData& objData, bool& isLandShape);
	//コライダー系のセット
	void SetCollider(Object* obj, const LevelData::ObjectData& objData, bool isSettingCollider);
	//引数によってObjectかTouchObjectを作る
	void CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, Model* model);



public:
	bool GetGameOver();
	bool GetGameClear();
};