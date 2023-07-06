#pragma once
#include"TextureManager.h"

//アフィン変換行列用
class ConstBuffTransform
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

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