#pragma once
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include "WindowsApp.h"
#include <DirectXMath.h>
#include <DirectXTex.h>
#include "KeyboardInput.h"
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include"Sound.h"

class Directx
{
private:
	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[5];
	D3D_FEATURE_LEVEL featureLevel;
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	// バックバッファ
	std::vector< ComPtr <ID3D12Resource>> backBuffers;
	// 1.リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrierDesc{};
	// フェンスの生成
	ComPtr < ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;
	ComPtr < ID3D12DescriptorHeap> dsvHeap = nullptr;
	//音
	IXAudio2MasteringVoice* masterVoice;


	Directx();

	//初期化系
	void InitializeDevice();
	void InitializeCommand();
	void InitializeSwapchain();
	void InitializeRendertargetView();
	void InitializeDepthBuffer();
	void InitializeFence();

public:
	//音
	ComPtr<IXAudio2> xAudio2;
	
	HRESULT result;
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr < ID3D12DescriptorHeap> rtvHeap = nullptr;
	// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;



	//関数
	static Directx& GetInstance();

	void DrawInitialize();

	void DrawUpdate(const XMFLOAT4& winRGBA = { 0.1f,0.25f,0.5f,0.0f });

	void DrawUpdate2();

	
};