#include "ModelManager.h"
#include "FbxLoader.h"
#include <sstream>


ModelManager::~ModelManager()
{
}

ModelManager& ModelManager::GetInstance()
{
	static ModelManager sInst;
	return sInst;
}

void ModelManager::Finalize()
{
	nameAndModels_.clear();
	nameAndModelFBXs_.clear();
	sameNameOtherModels_.clear();
}

IModel* ModelManager::LoadModel(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
{
	// 1行分の文字列をストリームに変換して解析しやすくする
	std::istringstream line_stream(fileName);

	// '/'区切りで行の先頭文字列を取得
	std::string modelType;
	std::string modelName;

	//モデル名とモデルのタイプをもらう('.'区切りで)
	getline(line_stream, modelName, '.');
	getline(line_stream, modelType, '.');

	//fbxなら
	if (modelType == "FBX" || modelType == "fbx")
	{
		//モデル名もらって読み込む
		return LoadModelFBX(modelName, smoothing, sameNameOtherModel);
	}

	//.fbx等がついてない場合はobjと判断
	return LoadModelObj(modelName, smoothing, sameNameOtherModel);
}

ModelObj* ModelManager::LoadModelObj(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<ModelObj>>::iterator it = nameAndModels_.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (!sameNameOtherModel && it != nameAndModels_.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<ModelObj> model = ModelObj::LoadFromOBJ(fileName, smoothing);

	ModelObj* ansM = model.get();

	//保存しておく
	if (sameNameOtherModel)
	{
		sameNameOtherModels_.push_back(std::move(model));
	}
	else
	{
		nameAndModels_.insert(std::make_pair(fileName, std::move(model)));
	}

	//ポインタ返す
	return ansM;
}

ModelFBX* ModelManager::LoadModelFBX(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs_.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (!sameNameOtherModel && it != nameAndModelFBXs_.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<ModelFBX> model = FbxLoader::GetInstance()->LoadModelFromFile(fileName, smoothing);

	ModelFBX* ansM = model.get();

	//保存しておく
	if (sameNameOtherModel)
	{
		sameNameOtherModels_.push_back(std::move(model));
	}
	else
	{
		nameAndModelFBXs_.insert(std::make_pair(fileName, std::move(model)));
	}

	//ポインタ返す
	return ansM;
}
