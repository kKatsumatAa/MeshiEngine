/*
* @file ConstBuffTransform.h
* @brief アフィン変換用の行列のcbuff等のセット
*/

#pragma once
#include"TextureManager.h"
#include<memory>
#include"Vec3.h"

//アフィン変換行列用
class ConstBuffTransform
{
protected://エイリアス
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
		XMMATRIX lightCamera;    //ライトカメラ行列
		Vec3 cameraPos;//カメラ座標（ワールド座標）
		float pad;
	};

public:
	const int8_t S_RESOURCE_DESC_HEIGHT_ = 1;
	const int8_t S_DEPTH_ARRAY_SIZE_ = 1;
	const int8_t S_MIP_LEVELS_ = 1;
	const int8_t S_SAMPLE_DESC_COUNT_ = 1;

private://描画用
	//バッファ
	ComPtr<ID3D12Resource> constBuffTransform_;
	ConstBufferDataTransform* constMapTransform_ = nullptr;//定数バッファのマッピング用ポインタ

public:
	//コンストラクタ
	ConstBuffTransform() { ; }
	//デストラクタ
	~ConstBuffTransform() { ; }

	//初期化
	void Initialize();
	//描画コマンド
	void DrawCommand(int32_t index);

public:
	//ビュープロジェクション行列セット
	void SetViewProjMat(const XMMATRIX& mat) { constMapTransform_->viewproj = mat; }
	//ライトのビュープロジェクション行列セット
	void SetLightViewProjMat(const XMMATRIX& mat) { constMapTransform_->lightCamera = mat; }
	//ワールド行列セット
	void SetWorldMat(const XMMATRIX& mat) { constMapTransform_->world = mat; }
	//カメラ位置セット
	void SetCameraPos(const Vec3& pos) { constMapTransform_->cameraPos = pos; }

	//ワールド行列取得
	const XMMATRIX& GetWorldMat() const { return constMapTransform_->world; }
	//ビュープロジェクション行列取得
	const XMMATRIX& GetViewProjMat()const { return constMapTransform_->viewproj; }
	//カメラ位置取得
	const Vec3& GetCameraPos()const { return constMapTransform_->cameraPos; }
};