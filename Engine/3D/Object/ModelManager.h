#pragma once
#include"Model.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//モデル名とobj
	std::map<std::string, std::unique_ptr<Model>>nameAndModels;
	//モデル名とfbx
	std::map<std::string, std::unique_ptr<ModelFBX>>nameAndModelFBXs;

public:


private:
	ModelManager() { ; }
	~ModelManager();

	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

public:
	static ModelManager& GetInstance();

	Model* LoadModel(std::string fileName, bool smoothing = false, bool modelType = false);
	ModelFBX* LoadModelFBX(std::string fileName);
};