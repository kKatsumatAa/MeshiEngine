#include "ModelFBX.h"
#include "DirectXWrapper.h"


/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string ModelFBX::S_TYPE_DIRECTORY_ = "fbx/";


ModelFBX::~ModelFBX()
{
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

uint64_t ModelFBX::GetBoneIndex(const std::string& name) const
{
	for (auto index : boneNodeIndices_)
	{
		if (nodes_[index].name.find(name.c_str()) != std::string::npos)
		{
			return index;
		}
	}

	return 0;
}

uint32_t ModelFBX::GetAnimIndex(const std::string& animName)
{
	uint32_t count = 0;
	for (auto animData : animationDatas_)
	{
		if (animData.name == animName)
		{
			return count;
		}

		count++;
	}

	//最後までなければ使えないインデックス
	return (uint32_t) INVALID_ANIME_INDEX_;
}

void ModelFBX::Initialize()
{
	isFbx_ = true;

	IModel::Initialize();

	scaleExtend_ = DEFAULT_SCALE_EXTEND_;
}

void ModelFBX::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}
