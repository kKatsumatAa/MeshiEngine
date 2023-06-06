#pragma once
#include"Model.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//ƒ‚ƒfƒ‹–¼‚Æobj
	std::map<std::string, Model*>nameAndModels;
	//ƒ‚ƒfƒ‹–¼‚Æfbx
	std::map<std::string, ModelFBX*>nameAndModelFBXs;

public:


private:
	ModelManager() { ; }
	~ModelManager() { ; }

	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

public:
	static ModelManager& GetInstance();

	Model* LoadModel(std::string fileName, bool smoothing = false, bool modelType = false);
	ModelFBX* LoadModelFBX(std::string fileName);
};