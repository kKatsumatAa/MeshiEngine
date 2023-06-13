#include "ModelManager.h"
#include "FbxLoader.h"


ModelManager::~ModelManager()
{
	nameAndModels.clear();
	nameAndModelFBXs.clear();
}

ModelManager& ModelManager::GetInstance()
{
	static ModelManager inst;
	return inst;
}

Model* ModelManager::LoadModel(std::string fileName, bool smoothing, bool modelType)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<Model>>::iterator it = nameAndModels.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModels.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<Model> model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//保存しておく
	nameAndModels.insert(std::make_pair(fileName,std::move(model_)));

	//保存したものを取得
	it = nameAndModels.find(fileName);

	//ポインタ返す
	return it->second.get();
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModelFBXs.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<ModelFBX> model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//保存しておく
	nameAndModelFBXs.insert(std::make_pair(fileName, std::move(model_)));

	//保存したものを取得
	it = nameAndModelFBXs.find(fileName);

	//ポインタ返す
	return it->second.get();
}
