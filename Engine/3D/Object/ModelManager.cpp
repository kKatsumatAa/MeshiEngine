#include "ModelManager.h"
#include "FbxLoader.h"


ModelManager::~ModelManager()
{
}

ModelManager& ModelManager::GetInstance()
{
	static ModelManager inst;
	return inst;
}

void ModelManager::Finalize()
{
	nameAndModels_.clear();
	nameAndModelFBXs_.clear();
}

Model* ModelManager::LoadModel(std::string fileName, bool smoothing, bool modelType)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<Model>>::iterator it = nameAndModels_.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModels_.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<Model> model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//保存しておく
	nameAndModels_.insert(std::make_pair(fileName,std::move(model_)));

	//保存したものを取得
	it = nameAndModels_.find(fileName);

	//ポインタ返す
	return it->second.get();
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//ファイル名から探す
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs_.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModelFBXs_.end())
	{
		return it->second.get();
	}

	//なければ読み込み
	std::unique_ptr<ModelFBX> model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//保存しておく
	nameAndModelFBXs_.insert(std::make_pair(fileName, std::move(model_)));

	//保存したものを取得
	it = nameAndModelFBXs_.find(fileName);

	//ポインタ返す
	return it->second.get();
}
