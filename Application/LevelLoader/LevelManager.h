#pragma once
#include"JsonLevelLoader.h"
#include"IObject3D.h"
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

private://ゲームオリジナル
	//武器のポインタ返す
	Weapon* GetChildWeapon(LevelData::ObjectData& objData);

	//地形オブジェクトとして使うかチェック
	void CheckLandShapeObject(const LevelData::ObjectData& objData, bool& isLandShape);

private:
	//コライダー系のセット
	void SetCollider(IObject3D* obj, const LevelData::ObjectData& objData, bool isSettingCollider
		, IModel* model);
	//引数によってObjectかTouchObjectを作る
	void CreateObjectOrTouchableObject(std::unique_ptr<IObject3D>& obj, LevelData::ObjectData& objData, bool isLandShape, IModel* model);
	//親オブジェクトのモデルのノードを親に設定
	void SetParentNode(IObject3D* obj, const LevelData::ObjectData& objData, IModel* model
		, IObject3D* child);

public:
	void SetLightManager(LightManager* lightManager) { lightManager_ = lightManager; }
	LightManager* GetLightManager() { return lightManager_; }
};