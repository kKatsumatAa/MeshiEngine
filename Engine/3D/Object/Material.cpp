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
ID3D12Device* Material::device = nullptr;

void Material::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!Material::device);

	Material::device = device;
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
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuff);
}

Material::~Material()
{

}

void Material::LoadTexture(const std::string& directoryPath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	// テクスチャなし
	if (textureFilename.size() == 0) {
		textureFilename = "image/white.png";
	}

	cpuDescHandleSRV = cpuHandle;
	gpuDescHandleSRV = gpuHandle;

	HRESULT result = S_FALSE;

	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ファイルパスを結合
	string filepath = directoryPath + textureFilename;

	const char* name = filepath.c_str();
	wchar_t wchar[128];
	ConstCharToWcharT(name, wchar);
	TextureManager::LoadGraph(wchar, textureHandle);
}

void Material::Update()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferDataMaterial2* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient;
		constMap->diffuse = diffuse;
		constMap->specular = specular;
		constMap->alpha = alpha;
		constBuff->Unmap(0, nullptr);
	}
}

const CD3DX12_GPU_DESCRIPTOR_HANDLE& Material::GetGpuHandle()
{
	gpuDescHandleSRV.ptr = textureHandle;
	return gpuDescHandleSRV;
}
