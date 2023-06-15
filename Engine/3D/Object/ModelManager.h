#pragma once
#include"Model.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//ƒ‚ƒfƒ‹–¼‚Æobj
	std::map<std::string, std::unique_ptr<Model>>nameAndModels_;
	//ƒ‚ƒfƒ‹–¼‚Æfbx
	std::map<std::string, std::unique_ptr<ModelFBX>>nameAndModelFBXs_;

public:


private:
	ModelManager() { ; }
	~ModelManager();

	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

public:
	static ModelManager& GetInstance();

	//ŒãŽn––
	void Finalize();

	Model* LoadModel(std::string fileName, bool smoothing = false, bool modelType = false);
	ModelFBX* LoadModelFBX(std::string fileName);
};