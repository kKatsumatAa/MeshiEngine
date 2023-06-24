#pragma once
#include "GausianBuffer.h"
#include <array>


class DirectXWrapper final
{
private:
	//成果物系
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator_ = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue_ = nullptr;
	ComPtr<ID3D12Resource> depthBuff_;
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
	//
	ComPtr < ID3D12DescriptorHeap> dsvHeap_ = nullptr;

	// フェンスの生成
	ComPtr < ID3D12Fence> fence_ = nullptr;
	uint64_t fenceVal_ = 0;

	// 1.リソースバリアの設定
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	//デバイス
	ComPtr<ID3D12Device> device_ = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList_ = nullptr;

	//記録時間（FPS固定
	std::chrono::steady_clock::time_point reference_;

	float clearColor_[4] = { 0,0,0,0 };

	//
	HRESULT result_;

private:
	DirectXWrapper();
	~DirectXWrapper();

	//初期化系
	void InitializeDevice();
	void InitializeCommand();
	void InitializeSwapchain();
	void InitializeRendertargetView();
	void InitializeDepthBuffer();
	void InitializeFence();

	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();

public:
	void Initialize();


	//コピーコンストラクタを無効
	DirectXWrapper(const DirectXWrapper& obj) = delete;
	//代入演算子も
	DirectXWrapper& operator=(const DirectXWrapper& obj) = delete;

	//関数
	static DirectXWrapper& GetInstance();

	void CommandReset();

	void DrawInitialize();

	//1枚目のテクスチャに描画
	void PreDraw();
	void PostDraw();


	//getter
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_.Get(); }
	uint64_t& GetFenceVal() { return fenceVal_; }
	ID3D12Fence* GetFence() { return fence_.Get(); }
	ID3D12DescriptorHeap* GetRtvheap() { return rtvHeap_.Get(); }
	float* GetClearColor() { return clearColor_; }
	std::vector< ComPtr <ID3D12Resource>>& GetBackBuffer() { return backBuffers_; }
	ID3D12DescriptorHeap* GetDSVHeap() { return dsvHeap_.Get(); }

	//バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers_.size(); }
};
//画像のロード（引数にバッファ設定）
void LoadPictureFromFile(const wchar_t* fileName, ComPtr<ID3D12Resource>& texBuff);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, uint32_t size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


