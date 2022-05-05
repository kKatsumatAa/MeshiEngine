#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include "WindowsApp.h"
#include <DirectXMath.h>
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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
	std::vector<ID3D12Resource*> backBuffers;
	// 1.リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrierDesc{};
	// フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;
	// パイプランステートの生成
	ID3D12PipelineState* pipelineState[2] = { nullptr };
	// ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//頂点データ
	XMFLOAT3 vertices[4] = {
		{-0.5f, -0.5f, 0.0f},	//左下
		{-0.5f, +0.5f, 0.0f},	//左上
		{+0.5f, -0.5f, 0.0f},	//右下
		{+0.5f, +0.5f, 0.0f}	//右上
	};
	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[1] = {
	{
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)
	};
	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParam = {};
	//定数バッファの生成
	ID3D12Resource* constBuffMaterial = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色(RGBA)
	};
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	XMFLOAT4 color2 = { 0,0,0,0 };
	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//インデックスデータ
	uint16_t indices[6] =
	{
		0,1,2,//三角形1つ目
		1,2,3,//三角形2つ目
	};

public:
	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;



	//関数
	Directx(const WindowsApp& win)
	{
#ifdef _DEBUG
		//デバッグレイヤーをオンに
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
#endif

		// DXGIファクトリーの生成
		result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
		assert(SUCCEEDED(result));
		// アダプターの列挙用
		std::vector<IDXGIAdapter4*> adapters;
		// ここに特定の名前を持つアダプターオブジェクトが入る
		IDXGIAdapter4* tmpAdapter = nullptr;
		// パフォーマンスが高いものから順に、全てのアダプターを列挙する
		for (UINT i = 0;
			dxgiFactory->EnumAdapterByGpuPreference(i,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
			i++) {
			// 動的配列に追加する
			adapters.push_back(tmpAdapter);
		}

		// 妥当なアダプタを選別する
		for (size_t i = 0; i < adapters.size(); i++) {
			DXGI_ADAPTER_DESC3 adapterDesc;
			// アダプターの情報を取得する
			adapters[i]->GetDesc3(&adapterDesc);
			// ソフトウェアデバイスを回避
			if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				// デバイスを採用してループを抜ける
				tmpAdapter = adapters[i];
				break;
			}
		}

		levels[0] = D3D_FEATURE_LEVEL_12_1;
		levels[1] = D3D_FEATURE_LEVEL_12_0;
		levels[2] = D3D_FEATURE_LEVEL_11_1;
		levels[3] = D3D_FEATURE_LEVEL_11_0;


		for (size_t i = 0; i < _countof(levels); i++) {
			// 採用したアダプターでデバイスを生成
			result = D3D12CreateDevice(tmpAdapter, levels[i],
				IID_PPV_ARGS(&device));
			if (result == S_OK) {
				// デバイスを生成できた時点でループを抜ける
				featureLevel = levels[i];
				break;
			}
		}

		// コマンドアロケータを生成
		result = device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator));
		assert(SUCCEEDED(result));
		// コマンドリストを生成
		result = device->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator, nullptr,
			IID_PPV_ARGS(&commandList));
		assert(SUCCEEDED(result));

		//コマンドキューを生成
		result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
		assert(SUCCEEDED(result));

		swapChainDesc.Width = 1280;
		swapChainDesc.Height = 720;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
		swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
		swapChainDesc.BufferCount = 2; // バッファ数を2つに設定
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		// スワップチェーンの生成
		result = dxgiFactory->CreateSwapChainForHwnd(
			commandQueue, win.hwnd, &swapChainDesc, nullptr, nullptr,
			(IDXGISwapChain1**)&swapChain);
		assert(SUCCEEDED(result));

		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
		rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // 裏表の2つ
		// デスクリプタヒープの生成
		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

		backBuffers.resize(swapChainDesc.BufferCount);

		// スワップチェーンの全てのバッファについて処理する
		for (size_t i = 0; i < backBuffers.size(); i++) {
			// スワップチェーンからバッファを取得
			swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
			// デスクリプタヒープのハンドルを取得
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
			// 裏か表かでアドレスがずれる
			rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
			// レンダーターゲットビューの設定
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
			// シェーダーの計算結果をSRGBに変換して書き込む
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			// レンダーターゲットビューの生成
			device->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
		}

		//フェンス生成
		result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	}

	void DrawInitialize()
	{
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

		//頂点バッファの設定
		D3D12_HEAP_PROPERTIES heapProp{};			//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC resDesc{};
		ResourceProperties(resDesc, sizeVB);

		//頂点バッファの生成
		ID3D12Resource* vertBuff = nullptr;
		BuffProperties(heapProp, resDesc, &vertBuff);

		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		XMFLOAT3* vertMap = nullptr;
		result = vertBuff->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(result));
		// 全頂点に対して
		for (int i = 0; i < _countof(vertices); i++) {
			vertMap[i] = vertices[i]; // 座標をコピー
		}
		// 繋がりを解除
		vertBuff->Unmap(0, nullptr);

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbView.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbView.StrideInBytes = sizeof(XMFLOAT3);


		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"BasicVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"BasicPS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));

		//ルートパラメータの設定
		rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
		rootParam.Descriptor.ShaderRegister = 0;//定数バッファ番号
		rootParam.Descriptor.RegisterSpace = 0;//デフォルト値
		rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える


		// パイプランステートの生成
		PipeLineState(D3D12_FILL_MODE_SOLID, pipelineState);

		PipeLineState(D3D12_FILL_MODE_WIREFRAME, pipelineState + 1);


		//03_02
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		ResourceProperties(cbResourceDesc, 
			((UINT)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial);
		//定数バッファのマッピング
		result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
		assert(SUCCEEDED(result));
		constBuffTransfer({ 1.0f,0,0,0.5f });

		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

		//リソース設定
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indices); i++)
		{
			indexMap[i] = indices[i];//インデックスをコピー
		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeIB;
	}

	void DrawUpdate(const XMFLOAT4& winRGBA)
	{
		// バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		barrierDesc.Transition.pResource = backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		// 2.描画先の変更
		rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		// レンダーターゲットビューのハンドルを取得
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 3.画面クリア R G B A
		FLOAT clearColor[] = { winRGBA.x,winRGBA.y,winRGBA.z,winRGBA.w };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void DrawUpdate2()
	{
		// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		// 命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);
		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// コマンドの実行完了を待つ
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = commandList->Reset(commandAllocator, nullptr);
		assert(SUCCEEDED(result));
	}



	void GraphicsCommand(const WindowsApp& win, const D3D12_VIEWPORT& viewPort, const int& pipelineNum,
		const bool& primitiveMode)
	{
		constBuffTransfer({ -0.001f,0.001f,0,0 });

		D3D_PRIMITIVE_TOPOLOGY primitive;

		if (!primitiveMode) primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		else               primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		// ビューポート設定コマンド
		viewport = viewPort;
		// ビューポート設定コマンドを、コマンドリストに積む
		commandList->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + win.window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + win.window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		commandList->SetPipelineState(pipelineState[pipelineNum]);
		commandList->SetGraphicsRootSignature(rootSignature);

		// プリミティブ形状の設定コマンド
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角か四角

		// 頂点バッファビューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//インデックスバッファビューの設定コマンド
		commandList->IASetIndexBuffer(&ibView);

		// 描画コマンド
		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画
	}


	void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState)
	{
		// シェーダーの設定
		pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
		pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
		pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
		pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

		// サンプルマスクの設定
		pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

		// ラスタライザの設定
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
		pipelineDesc.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
		pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

		Blend(D3D12_BLEND_OP_ADD, false, true);

		// 頂点レイアウトの設定
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

		// 図形の形状設定
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// その他の設定
		pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
		pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング


		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.pParameters = &rootParam;//ルートパラメータの先頭アドレス
		rootSignatureDesc.NumParameters = 1;//ルートパラメータ数
		// ルートシグネチャのシリアライズ
		ID3DBlob* rootSigBlob = nullptr;
		result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
			&rootSigBlob, &errorBlob);
		assert(SUCCEEDED(result));
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(result));
		rootSigBlob->Release();
		// パイプラインにルートシグネチャをセット
		pipelineDesc.pRootSignature = rootSignature;

		result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
		assert(SUCCEEDED(result));
	}

	void Blend(const D3D12_BLEND_OP& blendMode,
		const bool& Inversion = 0, const bool& Translucent = 0)
	{
		//共通設定
		D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
		blendDesc.BlendEnable = true;//ブレンドを有効
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
		//合成ごと
		blendDesc.BlendOp = blendMode;
		if (Inversion)//反転
		{
			blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
			blendDesc.DestBlend = D3D12_BLEND_ZERO;//使わない
		}
		else if (Translucent)//半透明
		{
			blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-デストカラーの値
			blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
		}
		else
		{
			blendDesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
			blendDesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
		}
	}

	void constBuffTransfer(const XMFLOAT4& plusRGBA)
	{
		color2.x += plusRGBA.x;
		color2.y += plusRGBA.y;
		color2.z += plusRGBA.z;
		color2.w += plusRGBA.w;

		//値を書き込むと自動的に転送される
		constMapMaterial->color = color2;//半透明の赤
	}

	void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size)
	{
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = size;						//頂点データ全体のサイズ
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	}

	void BuffProperties(D3D12_HEAP_PROPERTIES& heap,D3D12_RESOURCE_DESC& resource,
		ID3D12Resource** buff)
	{
		result = device->CreateCommittedResource(
			&heap,//ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&resource,//リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buff));
		assert(SUCCEEDED(result));
	}

	void Error(const bool& filed)
	{
		if (filed)
		{
			// errorBlobからエラー内容をstring型にコピー
			std::string error;
			error.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				error.begin());
			error += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(error.c_str());
			assert(0);
		}
	}
};