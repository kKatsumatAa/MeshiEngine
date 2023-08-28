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
	sameNameOtherModels_.clear();
}

IModel* ModelManager::LoadModel(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
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
		return LoadModelFBX(modelName, smoothing, sameNameOtherModel);
	}

	//.fbx�������ĂȂ��ꍇ��obj�Ɣ��f
	return LoadModelObj(modelName, smoothing, sameNameOtherModel);
}

ModelObj* ModelManager::LoadModelObj(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelObj>>::iterator it = nameAndModels_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (!sameNameOtherModel && it != nameAndModels_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelObj> model = ModelObj::LoadFromOBJ(fileName, smoothing);

	ModelObj* ansM = model.get();

	//�ۑ����Ă���
	if (sameNameOtherModel)
	{
		sameNameOtherModels_.push_back(std::move(model));
	}
	else
	{
		nameAndModels_.insert(std::make_pair(fileName, std::move(model)));
	}

	//�|�C���^�Ԃ�
	return ansM;
}

ModelFBX* ModelManager::LoadModelFBX(const std::string& fileName, bool smoothing, bool sameNameOtherModel)
{
	//�t�@�C��������T��
	std::map<std::string, std::unique_ptr<ModelFBX>>::iterator it = nameAndModelFBXs_.find(fileName);
	//���łɓǂݍ��܂�Ă����炻�̃��f���̃|�C���^��Ԃ�
	if (!sameNameOtherModel && it != nameAndModelFBXs_.end())
	{
		return it->second.get();
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<ModelFBX> model = FbxLoader::GetInstance()->LoadModelFromFile(fileName, smoothing);

	ModelFBX* ansM = model.get();

	//�ۑ����Ă���
	if (sameNameOtherModel)
	{
		sameNameOtherModels_.push_back(std::move(model));
	}
	else
	{
		nameAndModelFBXs_.insert(std::make_pair(fileName, std::move(model)));
	}

	//�|�C���^�Ԃ�
	return ansM;
}
