#include "ModelManager.h"
#include "FbxLoader.h"
#include <sstream>


ModelManager::~ModelManager()
{
}

ModelManager& ModelManager::GetInstance()
{
	static ModelManager sInst;
	return sInst;
}

void ModelManager::Finalize()
{
	nameAndModels_.clear();
	nameAndModelFBXs_.clear();
}

IModel* ModelManager::LoadModel(const std::string& fileName, bool smoothing)
{
	// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
	std::istringstream line_stream(fileName);

	// '/'��؂�ōs�̐擪��������擾
	std::string modelType;
	std::string modelName;

	//���f�����ƃ��f���̃^�C�v�����炤('.'��؂��)
	getline(line_stream, modelName, '.');
	getline(line_stream, modelType, '.');

	//fbx�Ȃ�
	if (modelType == "FBX" || modelType == "fbx")
	{
		//���f����������ēǂݍ���
		return LoadModelFBX(modelName);
	}

	//.fbx�������ĂȂ��ꍇ��obj�Ɣ��f
	return LoadModelObj(modelName);
}

ModelObj* ModelManager::LoadModelObj(const std::string& fileName, bool smoothing)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelObj>>::iterator it = nameAndModels_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModels_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelObj> model_ = ModelObj::LoadFromOBJ(fileName, smoothing);

	//�ۑ����Ă���
	nameAndModels_.insert(std::make_pair(fileName, std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModels_.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}

ModelFBX* ModelManager::LoadModelFBX(const std::string& fileName, bool smoothing)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (it != nameAndModelFBXs_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelFBX> model_ = FbxLoader::GetInstance()->LoadModelFromFile(fileName, smoothing);

	//�ۑ����Ă���
	nameAndModelFBXs_.insert(std::make_pair(fileName, std::move(model_)));

	//�ۑ��������̂��擾
	it = nameAndModelFBXs_.find(fileName);

	//�|�C���^�Ԃ�
	return it->second.get();
}
