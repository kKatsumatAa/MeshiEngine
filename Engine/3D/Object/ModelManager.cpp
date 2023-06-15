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
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<Model>>::iterator it = nameAndModels_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModels_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<Model> model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//�ۑ����Ă���
	nameAndModels_.insert(std::make_pair(fileName,std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModels_.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModelFBXs_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelFBX> model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//�ۑ����Ă���
	nameAndModelFBXs_.insert(std::make_pair(fileName, std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModelFBXs_.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}
