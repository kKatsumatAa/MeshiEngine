#include "ModelManager.h"
#include "FbxLoader.h"


ModelManager& ModelManager::GetInstance()
{
	static ModelManager inst;
	return inst;
}

Model* ModelManager::LoadModel(std::string fileName, bool smoothing, bool modelType)
{
	//ファイル名から探す
	std::map<std::string, Model*>::iterator it = nameAndModels.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModels.end())
	{
		return it->second;
	}

	//なければ読み込み
	Model* model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//保存しておく
	nameAndModels.insert(std::make_pair(fileName, model_));

	//ポインタ返す
	return model_;
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//ファイル名から探す
	std::map<std::string, ModelFBX*>::iterator it = nameAndModelFBXs.find(fileName);
	//すでに読み込まれていたらそのモデルのポインタを返す
	if (it != nameAndModelFBXs.end())
	{
		return it->second;
	}

	//なければ読み込み
	ModelFBX* model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//保存しておく
	nameAndModelFBXs.insert(std::make_pair(fileName, model_));

	//ポインタ返す
	return model_;
}
