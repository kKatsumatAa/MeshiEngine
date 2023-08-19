#pragma once
#include"JsonLevelLoader.h"
#include"Object.h"
#include"Camera.h"
#include"Gun.h"
#include"ObjectManager.h"


//jsonで読み込んだレベルを管理する
class LevelManager final
{
private://変数
	//地形オブジェクトとして使うか
	bool isLandShape_ = false;

	const std::string FILE_NAME_ = "level";

private:
	LightManager* lightManager_ = nullptr;
	float ambientColor_[3] = { 1,1,1 };
	float diffuseColor_[3] = { 1,1,1 };
	float specularColor_[3] = { 1,1,1 };
	float DirlightDir_[3] = { 0,-1.0f,1.0f };

	float pointPos_[3] = { 0,27.0f,-100.0f };


public:
	//オブジェクトのグループ名
	static const std::string S_OBJ_GROUP_NAME_;


private://関数
	LevelManager() { ; }
	~LevelManager();

public:
	//コピーコンストラクタ禁止
	LevelManager(const LevelManager& other) = delete;
	LevelManager& operator=(const LevelManager& other) = delete;

	static LevelManager& GetInstance();

	void StaticInitialize();
	
	/// <summary>
	///JsonLevelLoaderで読み込んだ内容を実際にエンジン上で読み込む
	/// </summary>
	/// <param name="fileIndex">ファイルの後につく数字</param>
	void LoadLevelData(int32_t fileIndex);

	//ファイル名によってクラス生成
	void LoadObj(LevelData::ObjectData& objData);
	//ライトを生成設定
	void LoadLight(LevelData::LightData& lightData);

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
	void CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, IModel* model);

public:
	void SetLightManager(LightManager* lightManager) { lightManager_ = lightManager; }
};