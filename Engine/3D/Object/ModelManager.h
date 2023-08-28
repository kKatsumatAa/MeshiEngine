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
	//�������O�̕ʂ̃C���X�^���X
	std::vector<std::unique_ptr<IModel>>sameNameOtherModels_;

public:


private:
	ModelManager() { ; }
	~ModelManager();

private:

	ModelObj* LoadModelObj(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);
	ModelFBX* LoadModelFBX(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);

public:
	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

	static ModelManager& GetInstance();

	//��n��
	void Finalize();

	/// <summary>
	///fbx�ł�obj�ł��ǂݍ��݁ifbx�̏ꍇ�� "xxx.fbx"�Ƃ���j
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	/// <param name="smoothing">�X���[�W���O</param>
	/// <param name="otherSameNameModel">�������O�̕ʂ̃C���X�^���X</param>
	/// <returns>���f���̃|�C���^</returns>
	IModel* LoadModel(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);
};