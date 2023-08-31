#include "ModelFBX.h"
#include "DirectXWrapper.h"


/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string ModelFBX::S_TYPE_DIRECTORY_ = "fbx/";


ModelFBX::~ModelFBX()
{
	//fbxシーンの解放（FBXLoaderのFinalize()の前じゃないとエラー出る）
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

int32_t ModelFBX::GetBoneIndex(const std::string& name) const
{
	int32_t count = 0;
	for (auto bone : bones_)
	{
		if (bone.name_.find(name.c_str()) != std::string::npos)
		{
			return count;
		}
		count++;
	}

	return -1;
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
