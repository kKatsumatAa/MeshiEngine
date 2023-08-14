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

void ModelFBX::Initialize()
{
	isFbx_ = true;

	IModel::Initialize();
}

void ModelFBX::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}
