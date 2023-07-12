#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "Vec3.h"
#include "WorldMat.h"
#include "Camera.h"
#include <functional>
#include "CollisionTypes.h"


struct LevelData
{
	//コライダーデータ
	struct ColliderData
	{
		//コライダー使うか
		bool isVailid = false;
		//中心座標
		Vec3 center = { 0,0,0 };
		//スケール
		Vec3 size = { 1,1,1 };
		//コライダータイプ
		CollisionShapeType colliderType = SHAPE_UNKNOWN;
		//地面や、壁として扱うか
		uint16_t attribute = 0;
	};

	//オブジェクト一個分のデータ
	struct ObjectData
	{
		//worldMat
		std::unique_ptr<WorldMat> worldMat;
		//ファイル名
		std::string fileName;
		//コライダーのデータ
		ColliderData colliderData;
		//子のデータ
		ObjectData* childData;
	};

	//カメラのデータ
	struct CameraData
	{
		//worldMat
		std::unique_ptr<Camera> camera;
		//ファイル名
		std::string fileName;
	};

	//オブジェクトの配列
	std::vector<std::unique_ptr<ObjectData>> objects;
	//カメラ
	std::vector<std::unique_ptr<CameraData>> cameras;
};

class JsonLevelLoader final
{
private:
	static const std::string S_DEFAULT_BASE_DIRECTORY_;
	static const std::string S_EXTENSION_;

public:
	//レベルデータ
	std::unique_ptr<LevelData> levelData_ = nullptr;

private:
	JsonLevelLoader() { ; }
	~JsonLevelLoader() { ; }

public:
	//コピーコンストラクタ禁止
	JsonLevelLoader& operator=(const JsonLevelLoader& inst) = delete;
	JsonLevelLoader(const JsonLevelLoader& inst) = delete;

	static JsonLevelLoader& Getinstance();
	void Initialize();

	//jsonファイル読み込み
	void LoadJsonFile(const std::string& fileName);

	//再帰的に子供を読み込み(オブジェクトデータ)
	void LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent = nullptr);
	//カメラのデータ読み込み
	void LoadCameraData(const nlohmann::json::iterator& object);
};