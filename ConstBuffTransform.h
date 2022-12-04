#pragma once
#include"TextureManager.h"

class ConstBuffTransform
{
private:
	//定数バッファ用データ構造
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D変換行列
	};

public:
	ComPtr < ID3D12Resource> constBuffTransform = nullptr;//定数バッファのGPUリソースのポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;//定数バッファのマッピング用ポインタ

	ConstBuffTransform(/*D3D12_RESOURCE_DESC& resDesc,*/ /*Directx& directx*/);
	void Initialize(Directx& directx);
};