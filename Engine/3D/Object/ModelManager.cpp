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
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<Model>>::iterator it = nameAndModels.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModels.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<Model> model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//�ۑ����Ă���
	nameAndModels.insert(std::make_pair(fileName,std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModels.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModelFBXs.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelFBX> model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//�ۑ����Ă���
	nameAndModelFBXs.insert(std::make_pair(fileName, std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModelFBXs.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}
