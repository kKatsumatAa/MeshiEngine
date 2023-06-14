#include "DirectXWrapper.h"
#include <thread>
#include <d3dx12.h>


void DirectXWrapper::InitializeDevice()
{
	// DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));
	// アダプターの列挙用
	std::vector< ComPtr<IDXGIAdapter4>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr < IDXGIAdapter4> tmpAdapter = nullptr;
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (uint32_t i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	// 妥当なアダプタを選別する
	for (int32_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		// アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);
		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// デバイスを採用してループを抜ける
			tmpAdapter = adapters[i].Get();
			break;
		}
	}

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int32_t i = 0; i < _countof(levels); i++) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXWrapper::InitializeCommand()
{
	// コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));
	// コマンドリストを生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
}

void DirectXWrapper::InitializeSwapchain()
{
	//スワップチェーン設定
	swapChainDesc.Width = (uint32_t)WindowsApp::GetInstance().window_width;
	swapChainDesc.Height = (uint32_t)WindowsApp::GetInstance().window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc.BufferCount = 2; // バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// スワップチェーンの生成
	ComPtr<IDXGISwapChain1> swapChain1;

	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(), WindowsApp::GetInstance().Gethwnd(), &swapChainDesc, nullptr, nullptr,
		&swapChain1);
	assert(SUCCEEDED(result));

	swapChain1.As(&swapChain);
}

void DirectXWrapper::InitializeRendertargetView()
{
	//デスクリプタヒープの設定
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // 裏表の2つ
	// デスクリプタヒープの生成
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()));

	//バックバッファ
	backBuffers.resize(swapChainDesc.BufferCount);

	//RTV生成
		// スワップチェーンの全てのバッファについて処理する
	for (int32_t i = 0; i < backBuffers.size(); i++) {
		// スワップチェーンからバッファを取得
		swapChain->GetBuffer((uint32_t)i, IID_PPV_ARGS(&backBuffers[i]));
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
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}


}

void DirectXWrapper::InitializeDepthBuffer()
{

	//06_01
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = (uint16_t)WindowsApp::GetInstance().window_width;//レンダーターゲットに合わせる
	depthResourceDesc.Height = (uint32_t)WindowsApp::GetInstance().window_height;//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル
	//深度値ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値クリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	//リソース生成
	result = device->CreateCommittedResource(
		&depthHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//<-ここが、_GENERIC_READになってた！！
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));
	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは一つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,//深度ビュー
		dsvHeap->GetCPUDescriptorHandleForHeapStart()//ヒープの先頭に作る
	);
}

void DirectXWrapper::InitializeFence()
{
	//フェンス生成
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

void DirectXWrapper::InitializeFixFPS()
{
	//現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::UpdateFixFPS()
{
	//1/60(s)ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60(s)よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60(s)（よりわずかに短い時間）経ってない場合
	if (elapsed < kMinCheckTime)
	{
		//1/60(s)経過するまで微小なスリープ繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinCheckTime)//
		{
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::Initialize()
{
	//FPS固定初期化
	InitializeFixFPS();

	//デバイス生成（ifdef系も関数にすると何故かエラーが起きる）
#ifdef _DEBUG
		//デバッグレイヤーをオンに
	ComPtr < ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);//重いので描画が変になった時のみ
	}
#endif
	InitializeDevice();
#ifdef _DEBUG
	ComPtr < ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf()))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);//やばいエラー時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);//エラー時止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);//警告時止まる
		infoQueue->Release();
	}

	//抑制するエラー
	D3D12_MESSAGE_ID denyIds[] = {
		//win11でのバグによるエラーメッセ
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};
	//抑制する表示レベル
	D3D12_MESSAGE_SEVERITY serverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(serverities);
	filter.DenyList.pSeverityList = serverities;
	//指定したエラーの表示を抑制
	infoQueue->PushStorageFilter(&filter);

#endif 

	//コマンド関連
	InitializeCommand();
	//スワップチェーン
	InitializeSwapchain();
	//レンダーターゲット
	InitializeRendertargetView();
	//深度バッファ
	InitializeDepthBuffer();
	//フェンス
	InitializeFence();
}

DirectXWrapper::DirectXWrapper()
{
}

DirectXWrapper::~DirectXWrapper()
{
}



DirectXWrapper& DirectXWrapper::GetInstance()
{
	static DirectXWrapper inst; // private なコンストラクタを呼び出す。
	return inst;
}

void DirectXWrapper::CommandReset()
{
	// キューをクリア
	result = commandAllocator->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void DirectXWrapper::DrawInitialize()
{

}

//一枚目のテクスチャに描画
void DirectXWrapper::PreDraw(const XMFLOAT4& winRGBA)
{
	// バックバッファの番号を取得(2つなので0番か1番)
	uint32_t bbIndex = swapChain->GetCurrentBackBufferIndex();

	rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	// レンダーターゲットビューのハンドルを取得
	rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

	//1.リソースバリアで書き込み可能に変更
	barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // バックバッファを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	commandList->ResourceBarrier(1, &barrierDesc);

	// 3 パス 目 
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(
		1, &rtvHandle, false, &dsvHandle
	);

	// 3.画面クリア R G B A
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//06_01(dsv)
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	//毎フレーム深度バッファの値が描画範囲で最も奥(1.0)にリセットされる

	// ビューポート設定コマンドを、コマンドリストに積む
	commandList->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (long)(scissorRect.left + WindowsApp::GetInstance().window_width); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (long)(scissorRect.top + WindowsApp::GetInstance().window_height); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXWrapper::PostDraw()
{
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
	commandList->ResourceBarrier(1, &barrierDesc);

	// 命令のクローズ
	result = commandList->Close();
	assert(SUCCEEDED(result));
	// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	// コマンドの実行完了を待つ
	commandQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//FPS固定
	UpdateFixFPS();

	//リセット
	CommandReset();

	// 画面に表示するバッファをフリップ(裏表の入替え)
	result = swapChain->Present(1, 0);
	assert(SUCCEEDED(result));
}

void LoadPictureFromFile(const wchar_t* fileName, ComPtr<ID3D12Resource>& texBuff)
{
	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	DirectXWrapper::GetInstance().result = LoadFromWICFile(
		fileName,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap生成
	DirectXWrapper::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(DirectXWrapper::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


	HRESULT result;

	//コピー先用
	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(uint64_t)metadata.width,
			(uint32_t)metadata.height,
			(uint16_t)metadata.arraySize,
			(uint16_t)metadata.mipLevels,
			1);

	// テクスチャバッファの生成
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(texBuff.GetAddressOf()));
	assert(SUCCEEDED(result));


	//アップロードバッファ
	// ヒープの設定
	uint64_t uploadSize = GetRequiredIntermediateSize(texBuff.Get(), 0, (uint32_t)metadata.mipLevels);

	D3D12_HEAP_PROPERTIES uploadHeapProp{};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC resDesc =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuff;

	// テクスチャバッファの生成
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&uploadHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuff));
	assert(SUCCEEDED(result));


	//サブリソース生成
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (int32_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// 全ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	//サブリソースを転送
	UpdateSubresources(
		DirectXWrapper::GetInstance().GetCommandList(),
		texBuff.Get(),
		uploadBuff.Get(),
		0,
		0,
		(uint32_t)metadata.mipLevels,
		subResourcesDatas.data()
	);

	//元データ解放
	scratchImg.Release();

	//バリアとフェンス
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff.Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//ここが重要
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//ここも重要

	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	DirectXWrapper::GetInstance().GetCommandList()->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { DirectXWrapper::GetInstance().GetCommandList() };
	DirectXWrapper::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//コマンド閉じる
	DirectXWrapper::GetInstance().GetCommandQueue()->Signal(DirectXWrapper::GetInstance().GetFence(),
		++DirectXWrapper::GetInstance().GetFenceVal());

	if (DirectXWrapper::GetInstance().GetFence()->GetCompletedValue() != DirectXWrapper::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		DirectXWrapper::GetInstance().GetFence()->SetEventOnCompletion(DirectXWrapper::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//バッファ解放
	/*uploadBuff->Release();
	uploadBuff.Reset();*/

	//コマンドリセット
	DirectXWrapper::GetInstance().CommandReset();
}

//------------------------------------------------------------------------------
void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const uint32_t& size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	DirectXWrapper::GetInstance().result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
}


