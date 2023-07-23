#include "Material.h"
#include <DirectXTex.h>
#include <cassert>
#include "TextureManager.h"
#include "Util.h"

using namespace DirectX;
using namespace std;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Material::sDevice_ = nullptr;

void Material::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!Material::sDevice_);

	Material::sDevice_ = device;
}

std::unique_ptr<Material> Material::Create()
{
	std::unique_ptr<Material> instance = std::make_unique<Material>();

	instance->Initialize();

	return std::move(instance);
}

void Material::Initialize()
{
	// 定数バッファの生成
	CreateConstantBuffer();
}

void Material::CreateConstantBuffer()
{
	// 定数バッファの生成
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(Material::ConstBufferDataMaterial2) + 0xff) & ~0xff/*256バイトアライメント*/);
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuff_);
}

Material::~Material()
{

}

void Material::LoadTexture(const std::string& directoryPath, const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	cpuDescHandleSRV_ = cpuHandle;
	gpuDescHandleSRV_ = gpuHandle;

	HRESULT result = S_FALSE;

	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ファイルパスを結合
	string filepath = directoryPath + textureFilename_;
	// テクスチャなし
	if (textureFilename_.size() == 0) {
		filepath = "white.png";
	}

	const char* NAME = filepath.c_str();
	wchar_t wchar[128];
	ConstCharToWcharT(NAME, wchar);
	TextureManager::LoadGraph(wchar, textureHandle_);
}

void Material::Update()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferDataMaterial2* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient_;
		constMap->diffuse = diffuse_;
		constMap->specular = specular_;
		constMap->alpha = alpha_;
		constBuff_->Unmap(0, nullptr);
	}
}

const CD3DX12_GPU_DESCRIPTOR_HANDLE& Material::GetGpuHandle()
{
	gpuDescHandleSRV_.ptr = textureHandle_;
	return gpuDescHandleSRV_;
}
