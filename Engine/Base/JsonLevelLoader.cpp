#include "JsonLevelLoader.h"
#include "Util.h"
#include "CollisionManager.h"
#include <fstream>
#include <sstream>

const std::string JsonLevelLoader::S_DEFAULT_BASE_DIRECTORY_ = "Resources/Level/levelData/";
const std::string JsonLevelLoader::S_EXTENSION_ = ".json";


JsonLevelLoader& JsonLevelLoader::Getinstance()
{
	static JsonLevelLoader sInst;
	return sInst;
}

void JsonLevelLoader::Initialize()
{
	levelData_.reset();
	levelData_ = std::make_unique<LevelData>();
}

void JsonLevelLoader::LoadJsonFile(const std::string& fileName)
{
	Initialize();

	//連結してフルパスを得る
	const std::string FULL_PATH = S_DEFAULT_BASE_DIRECTORY_ + fileName + S_EXTENSION_;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(FULL_PATH);
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

	//"name_"を文字列として取得
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

		//Camera
		if (type.compare("CAMERA") == 0)
		{
			LoadCameraData(object);
		}

		//ライト
		if (type.compare("LIGHT") == 0)
		{
			LoadLightData(object);
		}
	}
}

void JsonLevelLoader::LoadRecursiveChildrenData(const nlohmann::json::iterator& object, WorldMat* parent)
{
	//要素追加
	levelData_->objects.emplace_back(std::make_unique<LevelData::ObjectData>());
	//今追加した要素の参照を得る
	std::unique_ptr<LevelData::ObjectData>& objectData = levelData_->objects.back();

	if (object->contains("file_name"))
	{
		//ファイル名
		objectData->fileName = (*object)["file_name"];
	}

	//コライダーのパラメータ読み込み
	objectData->worldMat = std::make_unique<WorldMat>();
	// 親(一番目のオブジェクトはnullptrが入るように)
	objectData->worldMat->parent_ = parent;
	//平行移動
	objectData->worldMat->trans_ = GetTrans(object);
	//角度（うまくいってないかも）
	objectData->worldMat->rot_ = GetRot(object);
	//スケール
	objectData->worldMat->scale_ = GetScale(object);


	//ウェーブ番号
	if (object->contains("wave_num"))
	{
		std::string numStr = (*object)["wave_num"];
		objectData->waveNum = std::stoi(numStr);
	}

	//親ノード
	if (object->contains("parent_node"))
	{
		nlohmann::json parent_node = (*object)["parent_node"];
		objectData->parentNodeData.parentName = parent_node["parent_name"];
		objectData->parentNodeData.nodeName = parent_node["node_name"];
	}

	//当たり判定データ
	//トランスフォームのパラメータ読み込み
	nlohmann::json collider = (*object)["collider"];
	if (collider.size())
	{
		//コライダー種類
		std::string typeStr = collider["type"];
		if (typeStr == "SPHERE")
		{
			objectData->colliderData.colliderType = CollisionShapeType::COLLISIONSHAPE_SPHERE;
		}
		else if (typeStr == "MESH")
		{
			objectData->colliderData.colliderType = CollisionShapeType::COLLISIONSHAPE_MESH;
		}
		//判定属性
		std::string line = (std::string)collider["attribute"];
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		// ','区切りで行の先頭文字列を取得
		std::string key;
		uint16_t attribute = 0;
		while (std::getline(line_stream, key, ','))
		{
			if (key == "ALLIES")
			{
				attribute |= COLLISION_ATTR_ALLIES;
			}
			else if (key == "ENEMYS")
			{
				attribute |= COLLISION_ATTR_ENEMYS;
			}
			else if (key == "LANDSHAPE")
			{
				attribute |= COLLISION_ATTR_LANDSHAPE;
			}
			else if (key == "ITEMS")
			{
				attribute |= COLLISION_ATTR_ITEMS;
			}
		}
		objectData->colliderData.attribute = attribute;

		//中心位置
		objectData->colliderData.center.x_ = (float)collider["center"][1];
		objectData->colliderData.center.y_ = (float)collider["center"][2];
		objectData->colliderData.center.z_ = -(float)collider["center"][0];
		//スケール
		objectData->colliderData.size.x_ = (float)collider["size"][1];
		objectData->colliderData.size.y_ = (float)collider["size"][2];
		objectData->colliderData.size.z_ = (float)collider["size"][0];
	}

	//親がいたら、おやに自分のデータ入れる
	if (parent)
	{
		auto itr = levelData_->objects.end();
		std::advance(itr, -2);//endは何もなく、その一個前が今の自分が入ってるので二回戻す
		itr->get()->childData = objectData.get();
	}

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


//-----------------------------------------------------------------------
Vec3 JsonLevelLoader::GetRot(const nlohmann::json::iterator& object)
{
	//トランスフォームのパラメータ読み込み
	nlohmann::json transform = (*object)["transform"];

	Vec3 rot = {
			AngletoRadi(-(float)transform["rotation"][1]),
			AngletoRadi(-(float)transform["rotation"][2]),
			AngletoRadi((float)transform["rotation"][0])
	};
	return rot;
}

Vec3 JsonLevelLoader::GetRotDir(const nlohmann::json::iterator& object)
{
	//角度を得る
	WorldMat targetWorldMat;
	targetWorldMat.rot_ = GetRot(object);

	targetWorldMat.CalcWorldMat();
	//奥に向かうベクトルをターゲットの元とする
	Vec3 dir = { 0,0,1.0f };
	//読み込んだ角度でターゲットを回転
	Vec3xM4(dir, targetWorldMat.matWorld_, 0);


	return dir;
}

Vec3 JsonLevelLoader::GetTrans(const nlohmann::json::iterator& object)
{
	//トランスフォームのパラメータ読み込み
	nlohmann::json transform = (*object)["transform"];

	return { (float)transform["translation"][1], (float)transform["translation"][2], -(float)transform["translation"][0] };
}

Vec3 JsonLevelLoader::GetScale(const nlohmann::json::iterator& object)
{
	//トランスフォームのパラメータ読み込み
	nlohmann::json transform = (*object)["transform"];

	return{ (float)transform["scaling"][1],	(float)transform["scaling"][2],(float)transform["scaling"][0] };
}

//-------------------------------------------------------------------------------
//カメラのデータ読み込み
void JsonLevelLoader::LoadCameraData(const nlohmann::json::iterator& object)
{
	//要素追加
	levelData_->cameras.emplace_back(std::make_unique<LevelData::CameraData>());
	//今追加した要素の参照を得る
	std::unique_ptr<LevelData::CameraData>& objectData = levelData_->cameras.back();

	if (object->contains("file_name"))
	{
		//ファイル名
		objectData->fileName = (*object)["file_name"];
	}

	//カメラ作成
	objectData->camera = std::make_unique<Camera>();
	objectData->camera->Initialize();
	//平行移動
	objectData->camera->SetEye(GetTrans(object));
	//ターゲット
	objectData->camera->SetTarget(GetRotDir(object));
}

void JsonLevelLoader::LoadLightData(const nlohmann::json::iterator& object)
{
	//要素追加
	levelData_->lights.emplace_back(std::make_unique<LevelData::LightData>());
	//今追加した要素の参照を得る
	std::unique_ptr<LevelData::LightData>& objectData = levelData_->lights.back();

	if (object->contains("file_name"))
	{
		//ファイル名
		objectData->fileName = (*object)["file_name"];
	}

	//平行移動
	objectData->trans = GetTrans(object);
	//ターゲット
	objectData->dir = GetRotDir(object);
	//スケール(強さなどに使う)
	objectData->scale = GetScale(object);
}
