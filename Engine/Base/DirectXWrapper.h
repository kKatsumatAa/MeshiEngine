/*
* @file DirectXWrapper.h
* @brief DirectX関連の処理
*/

#pragma once
#include "GausianBuffer.h"
#include <array>
#include <list>


class DirectXWrapper final
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const int32_t S_BUFFER_ALIGNMENT_ = 0xff;

	static const int8_t S_DEPTH_VIEW_NUM_ = 2;
	static const int8_t S_DEPTH_BUFF_ARRAY_SIZE_ = 1;
	static const int8_t S_DEPTH_BUFF_SAMPLE_COUNT_ = 1;
	const float DEPTH_RATIO_MAX_ = 1.0f;

	const uint64_t SHADOW_DIFINITION_ = 1280;

	static const int8_t S_CLEAR_COLOR_NUM_ = 4;
public:
	static const int8_t S_DEFAULT_RESOURCE_HEIGHT_ = 1;
	static const int8_t S_DEFAULT_DEPTH_ARRAY_SIZE_ = 1;
	static const int8_t S_DEFAULT_MIP_LEVELS_ = 1;
	static const int8_t S_DEFAULT_SAMPLE_COUNT_ = 1;
public:
	const float FPS_ = 60.0f;
	const float FPS_ADD_LITTLE_ = 65.0f;
	const float MILLION_ = 1000000.0f;


private:
	bool isFirstFrame_ = true;

private:
	//成果物系
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue_ = nullptr;
	ComPtr < ID3D12CommandQueue> texCommandQueue_ = nullptr;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_;
	//シャドウマップ用深度バッファ
	ComPtr<ID3D12Resource> lightDepthBuff_;
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	ComPtr<IDXGISwapChain4> swapChain_;
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	ComPtr < ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	// レンダーターゲットビューのハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	// バックバッファ
	std::vector< ComPtr <ID3D12Resource>> backBuffers_;
	//深度バッファビュー用
	ComPtr < ID3D12DescriptorHeap> dsvHeap_ = nullptr;

	// フェンスの生成
	ComPtr < ID3D12Fence> fence_ = nullptr;
	uint64_t fenceVal_ = 0;
	ComPtr < ID3D12Fence> texFence_ = nullptr;
	uint64_t texFenceVal_ = 0;

	// 1.リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	//デバイス
	ComPtr<ID3D12Device> device_ = nullptr;

	ComPtr < ID3D12CommandAllocator> commandAllocator_ = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList_ = nullptr;
	ComPtr < ID3D12CommandAllocator> texCommandAllocator_ = nullptr;
	ComPtr < ID3D12GraphicsCommandList> texCommandList_ = nullptr;

	//記録時間（FPS固定
	std::chrono::steady_clock::time_point reference_;

	float clearColor_[S_CLEAR_COLOR_NUM_] = { 0,0,0,0 };

	//
	HRESULT result_;

	//テクスチャアップロード用のバッファ
	std::list<ComPtr<ID3D12Resource>> texUploadBuff_;

private:
	//コンストラクタ
	DirectXWrapper();
	//デストラクタ
	~DirectXWrapper();

	//デバイス初期化
	void InitializeDevice();
	//コマンド初期化
	void InitializeCommand();
	//スワップチェーン初期化
	void InitializeSwapchain();
	//レンダーターゲット初期化
	void InitializeRendertargetView();
	//深度バッファ初期化
	void InitializeDepthBuffer();
	//フェンス初期化
	void InitializeFence();

	//FPS固定初期化
	void InitializeFixFPS();

private:
	//FPS固定更新
	void UpdateFixFPS();

	//コマンド系の作成
	void CreateCommandLA(ID3D12CommandAllocator** texCommandAllocator,
		ID3D12GraphicsCommandList** texCommandList);

public:
	//初期化
	void Initialize();

	//コピーコンストラクタを無効
	DirectXWrapper(const DirectXWrapper& obj) = delete;
	//代入演算子も
	DirectXWrapper& operator=(const DirectXWrapper& obj) = delete;

	//関数
	static DirectXWrapper& GetInstance();

	//コマンドのリセット
	void CommandReset();

	//テクスチャを描画とは別のコマンドリストでアップするコマンド
	void UpLoadTexture();

	//影用の深度描画の前の処理
	void PreShadowDraw();
	//影用の深度描画の後の処理
	void PostShadowDraw();

	//描画の前の処理
	void PreDraw();
	//描画の後の処理
	void PostDraw();

	//リソースバリア
	void ResourceBarrier(D3D12_RESOURCE_STATES beforeState,
		D3D12_RESOURCE_STATES afterState, ID3D12Resource* buff);

public:
	//デバイスの取得
	ID3D12Device* GetDevice() const { return device_.Get(); }
	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	//テクスチャ用のコマンドリスト取得
	ID3D12GraphicsCommandList* GetTexCommandList() const { return texCommandList_.Get(); }
	//コマンドキュー取得
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_.Get(); }
	//fenceValueを取得
	uint64_t& GetFenceVal() { return fenceVal_; }
	//フェンス取得
	ID3D12Fence* GetFence() { return fence_.Get(); }
	//RTVのヒープ取得
	ID3D12DescriptorHeap* GetRtvheap() { return rtvHeap_.Get(); }
	//クリアカラー取得
	float* GetClearColor() { return clearColor_; }
	//バックバッファの配列取得
	std::vector< ComPtr <ID3D12Resource>>& GetBackBuffer() { return backBuffers_; }
	//DSVのヒープ取得
	ID3D12DescriptorHeap* GetDSVHeap() { return dsvHeap_.Get(); }
	//深度バッファ取得
	ID3D12Resource* GetDepthBuff() { return depthBuff_.Get(); }
	//シャドウマップ用の深度バッファ取得
	ID3D12Resource* GetLightDepthBuff() { return lightDepthBuff_.Get(); }
	//テクスチャのアップロード用のバッファのポインタのポインタ取得
	ID3D12Resource** GetTexUploadBuffPP() { return texUploadBuff_.back().GetAddressOf(); }
	//テクスチャのアップロード用のバッファのポインタ取得
	ID3D12Resource* GetTexUploadBuffP() { return texUploadBuff_.back().Get(); }
	//テクスチャのアップロード用のバッファのemplace_back()
	void EmplaceBackUploadBuff() { texUploadBuff_.emplace_back(); }

	//バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers_.size(); }

	//クリアカラーセット
	void SetClearColor(float clearColor[S_CLEAR_COLOR_NUM_]);
};
//画像のロード（引数にバッファ設定）
void LoadPictureFromFile(const char* fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texBuff);
//リソースの設定
void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, uint32_t size);
//バッファの設定
void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


