#pragma once
#include"TextureManager.h"

//アフィン変換行列用
class ConstBuffTransform
{
private:
	//定数バッファ用データ構造
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //ビュープロジェクション行列
		XMMATRIX world;    //ワールド行列
		XMFLOAT3 cameraPos;//カメラ座標（ワールド座標）
	};

public:
	ComPtr < ID3D12Resource> constBuffTransform_ = nullptr;//定数バッファのGPUリソースのポインタ
	ConstBufferDataTransform* constMapTransform_ = nullptr;//定数バッファのマッピング用ポインタ

	ConstBuffTransform();
	void Initialize();
};