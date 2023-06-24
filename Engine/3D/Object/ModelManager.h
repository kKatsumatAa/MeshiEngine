#pragma once
#include"Model.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//モデル名とobj
	std::map<std::string, std::unique_ptr<Model>>nameAndModels_;
	//モデル名とfbx
	std::map<std::string, std::unique_ptr<ModelFBX>>nameAndModelFBXs_;

public:


private:
	ModelManager() { ; }
	~ModelManager();

public:
	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

	static ModelManager& GetInstance();

	//後始末
	void Finalize();

	Model* LoadModel(std::string fileName, bool smoothing = false, bool modelType = false);
	ModelFBX* LoadModelFBX(std::string fileName);
};