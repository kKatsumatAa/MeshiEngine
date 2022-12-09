#include "Light.h"

ID3D12Device* Light::device_ = nullptr;


void Light::StaticInitialize()
{
	//再初期化をチェック
	assert(!Light::device_);
	//nullptrチェック
	assert(Directx::GetInstance().GetDevice());
	//セット
	Light::device_ = Directx::GetInstance().GetDevice();
}

void Light::Initialize()
{
	//ヒープ設定
	UINT sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(ConstBufferData));
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, constBuff.GetAddressOf());

	//定数バッファへデータ転送
	TransferConstBuffer();
}

void Light::TransferConstBuffer()
{
	HRESULT result;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->lightv = -lightdir;
		constMap->lightColor = lightColor;
		constBuff->Unmap(0, nullptr);
	}
}

void Light::SetLightDir(const XMVECTOR& lightdir)
{
	//正規化してセット
	this->lightdir = XMVector3Normalize(lightdir);
	dirty = true;
}

void Light::SetLightColor(const XMFLOAT3& lightcolor)
{
	this->lightColor = lightColor;
	dirty = true;
}

void Light::Update()
{
	//値の更新があった時だけ定数バッファに転送
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(UINT rootParamaterIndex)
{
	//定数バッファビューをセット
	Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParamaterIndex, constBuff->GetGPUVirtualAddress()
	);
}

Light* Light::Create()
{
	//3Dオブジェクトのインスタンスを生成
	Light* instance = new Light();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}
