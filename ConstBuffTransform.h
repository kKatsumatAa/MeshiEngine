#pragma once
#include"DirectX.h"

class ConstBuffTransform
{
private:
	//定数バッファ用データ構造
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D変換行列
	};
	//05_02
	
	/*HRESULT result;
	ID3D12Device* device = nullptr;*/
	//D3D12_RESOURCE_DESC resDesc{};

public:
	ID3D12Resource* constBuffTransform = nullptr;//定数バッファのGPUリソースのポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;//定数バッファのマッピング用ポインタ

	ConstBuffTransform(/*D3D12_RESOURCE_DESC& resDesc,*/ /*Directx& directx*/);
	void Initialize(Directx& directx);
};