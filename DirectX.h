#pragma once
#include "GausianBuffer.h"
#include <array>


class Directx final
{
private:
	//成果物系
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
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

	FLOAT clearColor[4] = { 0.1f,0.25f, 0.5f,0.0f };

	//ポストエフェクト用
	D3D12_CPU_DESCRIPTOR_HANDLE peraHandle;
	std::array<ComPtr<ID3D12Resource>, 2> _peraResource;
	ComPtr<ID3D12Resource> _peraResource2;
	ComPtr<ID3D12DescriptorHeap> _peraRTVHeap;//レンダーターゲット用
	ComPtr<ID3D12DescriptorHeap> _peraSRVHeap;//テクスチャ用
	bool isPeraClear = false;

	GausianBuffer gausianBuff;

	//ガラス
	ComPtr<ID3D12Resource>_effectTexBuffer;


private:
	Directx();
	~Directx();

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
	Directx(const Directx& obj) = delete;
	//代入演算子も
	Directx& operator=(const Directx& obj) = delete;

	//関数
	static Directx& GetInstance();

	void CommandReset();

	void DrawInitialize();

	//1枚目のテクスチャに描画
	void DrawUpdate(const XMFLOAT4& winRGBA = { 0.1f,0.25f,0.5f,0.0f });
	void DrawUpdate2();

	//2枚目のテクスチャに描画
	void PreDrawToPera();
	void PostDrawToPera();

	//実際に描画
	void PreDrawToPera2();
	void PostDrawToPera2();


	//getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	ComPtr<ID3D12DescriptorHeap> GetPeraSRVHeap() const { return _peraSRVHeap; }
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue.Get(); }
	UINT64& GetFenceVal() { return fenceVal; }
	ID3D12Fence* GetFence() { return fence.Get(); }

	//バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers.size(); }

	//ガラスフィルターのバッファ生成
	void GlassFilterBuffGenerate(const wchar_t* fileName);
};
//画像のロード（引数にバッファ設定）
void LoadPictureFromFile(const wchar_t* fileName, ComPtr<ID3D12Resource>& texBuff);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


