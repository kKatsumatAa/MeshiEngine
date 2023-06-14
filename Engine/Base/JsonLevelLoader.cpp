#include "JsonLevelLoader.h"



JsonLevelLoader& JsonLevelLoader::Getinstance()
{
	static JsonLevelLoader inst;
	return inst;
}

void JsonLevelLoader::Initialize()
{
	levelData = std::make_unique<LevelData>();
}

void JsonLevelLoader::LoadJsonFile(const std::string& fileName)
{
	//連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータ
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	//level

	//"objects"の全オブジェクトを走査
	for (nlohmann::json::iterator object = deserialized["objects"].begin(); object != deserialized["objects"].end(); object++)
	{
		assert(object->contains("type"));

		//種別を取得
		std::string type = (*object)["type"].get<std::string>();

		//種類ごとの処理
		//Mesh
		if (type.compare("MESH") == 0)
		{
			//再帰的
			LoadRecursiveChildrenData(object);

		}
	}
}

void JsonLevelLoader::LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent)
{
	//要素追加
	levelData->objects.emplace_back(std::make_unique<LevelData::ObjectData>());
	//今追加した要素の参照を得る
	std::unique_ptr<LevelData::ObjectData>& objectData = levelData->objects.back();

	if (object->contains("file_name"))
	{
		//ファイル名
		objectData->fileName = (*object)["file_name"];
	}

	//トランスフォームのパラメータ読み込み
	nlohmann::json transform = (*object)["transform"];

	//コライダーのパラメータ読み込み
	objectData->worldMat = std::make_unique<WorldMat>();
	// 親(一番目のオブジェクトはnullptrが入るように)
	objectData->worldMat->parent = parent;
	//平行移動
	objectData->worldMat->trans.x = (float)transform["translation"][1];
	objectData->worldMat->trans.y = (float)transform["translation"][2];
	objectData->worldMat->trans.z = -(float)transform["translation"][0];
	//角度（うまくいってないかも）
	objectData->worldMat->rot.x = -(float)transform["rotation"][1];
	objectData->worldMat->rot.y = -(float)transform["rotation"][2];
	objectData->worldMat->rot.z = (float)transform["rotation"][0];
	//スケール
	objectData->worldMat->scale.x = (float)transform["scaling"][1];
	objectData->worldMat->scale.y = (float)transform["scaling"][2];
	objectData->worldMat->scale.z = (float)transform["scaling"][0];

	//子がいたら
	if ((*object).contains("children"))
	{
		//子をfor文で走査
		for (nlohmann::json::iterator child = (*object)["children"].begin(); child != (*object)["children"].end(); child++)
		{
			//子を再帰で走査
			LoadRecursiveChildrenData(child, objectData->worldMat.get());
		}
	}
}
