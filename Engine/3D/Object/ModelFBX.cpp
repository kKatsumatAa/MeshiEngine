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
