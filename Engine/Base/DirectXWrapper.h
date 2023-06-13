#pragma once
#include "GausianBuffer.h"
#include <array>


class DirectXWrapper final
{
private:
	//成果物系
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12Resource> depthBuff;
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr < ID3D12DescriptorHeap> rtvHeap = nullptr;
	// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	// バックバッファ
	std::vector< ComPtr <ID3D12Resource>> backBuffers;
	//
	ComPtr < ID3D12DescriptorHeap> dsvHeap = nullptr;

	// フェンスの生成
	ComPtr < ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	// 1.リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrierDesc{};



	//外部で参照
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;

	//記録時間（FPS固定
	std::chrono::steady_clock::time_point reference_;

	FLOAT clearColor[4] = { 0,0,0,0 };


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

	//ガラス
	bool CreateEffectBufferAndView(const wchar_t* fileName);

public:
	//
	HRESULT result;

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
	void PreDraw(const XMFLOAT4& winRGBA = { 0,0,0,0 });
	void PostDraw();


	//getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	//ComPtr<ID3D12DescriptorHeap> GetPeraSRVHeap() const { return _peraSRVHeap; }
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue.Get(); }
	UINT64& GetFenceVal() { return fenceVal; }
	ID3D12Fence* GetFence() { return fence.Get(); }
	ID3D12DescriptorHeap* GetRtvheap() { return rtvHeap.Get(); }
	FLOAT* GetClearColor() { return clearColor; }
	std::vector< ComPtr <ID3D12Resource>>& GetBackBuffer() { return backBuffers; }
	ID3D12DescriptorHeap* GetDSVHeap() { return dsvHeap.Get(); }

	//バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers.size(); }
};
//画像のロード（引数にバッファ設定）
void LoadPictureFromFile(const wchar_t* fileName, ComPtr<ID3D12Resource>& texBuff);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


