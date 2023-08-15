#pragma once
#include"TextureManager.h"
#include<memory>
#include"Vec3.h"

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
		Vec3 cameraPos;//カメラ座標（ワールド座標）
		float pad;
	};


public:
	//保存しておくデータ
	struct ConstBuffTransformSaveData
	{
		bool isUsing = false;//今使用されているか
		ConstBufferDataTransform* constMapTransform = nullptr;//マッピング
		ComPtr<ID3D12Resource> constBuffTransform = nullptr;//定数バッファのGPUリソースのポインタ
	};
	//描画時に使うデータ
	struct ConstBuffTransformDrawData
	{
		D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle;//gpuのハンドル
		ConstBufferDataTransform* constMapTransform = nullptr;//マッピング
	};
	//データまとめ
	struct ConstBuffTransformAllData
	{
		ConstBuffTransformSaveData saveData;
		ConstBuffTransformDrawData drawData;
	};

private://管理用静的変数
	static D3D12_DESCRIPTOR_RANGE sDescRange_;
	//メモリ管理用
	static std::map<uint64_t, ConstBuffTransformSaveData> sCbtDatas_;

private://描画用
	//描画時にセットするgpuハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle_;
	ConstBufferDataTransform* constMapTransform_ = nullptr;//定数バッファのマッピング用ポインタ

public:
	ConstBuffTransform() { ; }
	~ConstBuffTransform();

	static void StaticInitialize();
	void Initialize();
	void DrawCommand(int32_t index);

public:
	void SetViewProjMat(const XMMATRIX& mat) { constMapTransform_->viewproj = mat; }
	void SetWorldMat(const XMMATRIX& mat) { constMapTransform_->world = mat; }
	void SetCameraPos(const Vec3& pos) { constMapTransform_->cameraPos = pos; }

public:
	//バッファを作成して登録
	static ConstBuffTransformDrawData CreateBuffAndAdd();

private:
	//保存してあればデータを返す
	static bool GetSaveData(ConstBuffTransformDrawData& data);
	//新たにバッファを作成して返す
	static ConstBuffTransformDrawData CreateBuff();
	static ConstBuffTransformAllData CreateBuffInternal();

public:
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetCBVGpuHandle() { return cbvGPUHandle_; }
	static const D3D12_DESCRIPTOR_RANGE& GetDescRange() { return sDescRange_; }
};