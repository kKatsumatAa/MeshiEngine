#pragma once
#include"ModelObj.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//���f������obj
	std::map<std::string, std::unique_ptr<ModelObj>>nameAndModels_;
	//���f������fbx
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

	//��n��
	void Finalize();

	//fbx�ł�obj�ł��ǂݍ��݁ifbx�̏ꍇ�� "xxx.fbx"�Ƃ���j
	IModel* LoadModel(const std::string& fileName, bool smoothing = false);
};