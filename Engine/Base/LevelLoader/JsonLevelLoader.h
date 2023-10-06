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

	//親ノードデータ
	struct ParentNodeData
	{
		std::string parentName;
		std::string nodeName;
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
		//ウェーブ番号
		int32_t waveNum = -1;
		//親ノード名
		ParentNodeData parentNodeData;
	};

	//カメラのデータ
	struct CameraData
	{
		//カメラ
		std::unique_ptr<Camera> camera;
		//ファイル名
		std::string fileName;
	};

	//ライトのデータ
	struct LightData
	{
		//位置
		Vec3 trans;
		//方向
		Vec3 dir;
		//スケール(強さなどに使う)
		Vec3 scale;
		//ファイル名
		std::string fileName;
	};

	//オブジェクトの配列
	std::vector<std::unique_ptr<ObjectData>> objects;
	//カメラ
	std::vector<std::unique_ptr<CameraData>> cameras;
	//ライト
	std::vector<std::unique_ptr<LightData>> lights;
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

private:
	//角度を得る
	Vec3 GetRot(const nlohmann::json::iterator& object);
	//角度を取得し、それを使いベクトルを得る
	Vec3 GetRotDir(const nlohmann::json::iterator& object);
	//位置を得る
	Vec3 GetTrans(const nlohmann::json::iterator& object);
	//スケールを得る
	Vec3 GetScale(const nlohmann::json::iterator& object);


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
	//ライトのデータ読み込み
	void LoadLightData(const nlohmann::json::iterator& object);
};