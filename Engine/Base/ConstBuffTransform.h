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
		Vec3 cameraPos;//カメラ座標（ワールド座標）
		float pad;
	};

private://描画用
	//バッファ
	ComPtr<ID3D12Resource> constBuffTransform_;
	ConstBufferDataTransform* constMapTransform_ = nullptr;//定数バッファのマッピング用ポインタ

public:
	ConstBuffTransform() { ; }
	~ConstBuffTransform() { ; }

	void Initialize();
	void DrawCommand(int32_t index);

public:
	void SetViewProjMat(const XMMATRIX& mat) { constMapTransform_->viewproj = mat; }
	void SetWorldMat(const XMMATRIX& mat) { constMapTransform_->world = mat; }
	void SetCameraPos(const Vec3& pos) { constMapTransform_->cameraPos = pos; }

	const XMMATRIX& GetWorldMat() const{ return constMapTransform_->world; }
	const XMMATRIX& GetViewProjMat()const { return constMapTransform_->viewproj; }
	const Vec3& GetCameraPos()const { return constMapTransform_->cameraPos; }
};