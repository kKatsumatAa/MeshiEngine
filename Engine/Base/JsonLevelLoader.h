#pragma once
#include <string>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "Vec3.h"
#include "WorldMat.h"

struct LevelData
{
	//オブジェクト一個分のデータ
	struct ObjectData
	{
		//worldMat
		std::unique_ptr<WorldMat> worldMat;
		//ファイル名
		std::string fileName;
	};

	//オブジェクトの配列
	std::vector<std::unique_ptr<ObjectData>> objects;
};

class JsonLevelLoader final
{
private:
	std::string kDefaultBaseDirectory = "Resources/Level/";
	std::string kExtension = ".json";

public:
	//レベルデータ
	std::unique_ptr<LevelData> levelData;

private:
	JsonLevelLoader() { ; }
	~JsonLevelLoader() { ; }

	//コピーコンストラクタ禁止
	JsonLevelLoader& operator=(const JsonLevelLoader& inst) = delete;
	JsonLevelLoader(const JsonLevelLoader& inst) = delete;

public:
	static JsonLevelLoader& Getinstance();
	void Initialize();

	//jsonファイル読み込み
	void LoadJsonFile(std::string fileName);

	//再帰的に子供を読み込み
	void LoadRecursiveChildrenData(nlohmann::json::iterator object, WorldMat* worldMat = nullptr);
};