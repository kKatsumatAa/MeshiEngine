#include "ModelFBX.h"
#include "DirectXWrapper.h"


/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
const std::string ModelFBX::S_TYPE_DIRECTORY_ = "fbx/";


ModelFBX::~ModelFBX()
{
	//fbx�V�[���̉���iFBXLoader��Finalize()�̑O����Ȃ��ƃG���[�o��j
	fbxScene_->Destroy();
}

const Node* ModelFBX::GetNode(const std::string& name)
{
	for (auto& nodeL : nodes_)
	{
		if (nodeL.name.find(name) != std::string::npos)
		{
			return &nodeL;
		}
	}

	return nullptr;
}

void ModelFBX::Initialize()
{
	isFbx_ = true;

	IModel::Initialize();

	scaleExtend_ = 0.01f;
}

void ModelFBX::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}
