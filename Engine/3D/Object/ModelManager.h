#pragma once
#include"ModelObj.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//モデル名とobj
	std::map<std::string, std::unique_ptr<ModelObj>>nameAndModels_;
	//モデル名とfbx
	std::map<std::string, std::unique_ptr<ModelFBX>>nameAndModelFBXs_;

public:


private:
	ModelManager() { ; }
	~ModelManager();

private:

	ModelObj* LoadModelObj(const std::string& fileName, bool smoothing = false);
	ModelFBX* LoadModelFBX(const std::string& fileName, bool smoothing = false);

public:
	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

	static ModelManager& GetInstance();

	//後始末
	void Finalize();

	//fbxでもobjでも読み込み（fbxの場合は "xxx.fbx"とする）
	IModel* LoadModel(const std::string& fileName, bool smoothing = false);
};