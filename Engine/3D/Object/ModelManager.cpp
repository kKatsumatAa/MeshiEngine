#include "ModelManager.h"
#include "FbxLoader.h"


ModelManager& ModelManager::GetInstance()
{
	static ModelManager inst;
	return inst;
}

Model* ModelManager::LoadModel(std::string fileName, bool smoothing, bool modelType)
{
	//�t�@�C��������T��
	std::map<std::string, Model*>::iterator it = nameAndModels.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModels.end())
	{
		return it->second;
	}

	//�Ȃ���Γǂݍ���
	Model* model_ = Model::LoadFromOBJ(fileName, smoothing, modelType);

	//�ۑ����Ă���
	nameAndModels.insert(std::make_pair(fileName, model_));

	//�|�C���^�Ԃ�
	return model_;
}

ModelFBX* ModelManager::LoadModelFBX(std::string fileName)
{
	//�t�@�C��������T��
	std::map<std::string, ModelFBX*>::iterator it = nameAndModelFBXs.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModelFBXs.end())
	{
		return it->second;
	}

	//�Ȃ���Γǂݍ���
	ModelFBX* model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	//�ۑ����Ă���
	nameAndModelFBXs.insert(std::make_pair(fileName, model_));

	//�|�C���^�Ԃ�
	return model_;
}
