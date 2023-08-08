#include "DirectXWrapper.h"
#include <thread>
#include <d3dx12.h>
#include "Util.h"

using namespace DirectX;
using namespace Microsoft::WRL;


void DirectXWrapper::InitializeDevice()
{
	// DXGIファクトリーの生成
	result_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result_));
	// アダプターの列挙用
	std::vector< ComPtr<IDXGIAdapter4>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr < IDXGIAdapter4> tmpAdapter = nullptr;
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (uint32_t i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
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
		result_ = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result_ == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXWrapper::InitializeCommand()
{
	// 描画コマンド系を生成
	CreateCommandLA(commandAllocator_.GetAddressOf(), commandList_.GetAddressOf());

	//テクスチャのコマンド系
	CreateCommandLA(texCommandAllocator_.GetAddressOf(), texCommandList_.GetAddressOf());

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result_));
	//テクスチャ用も
	result_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&texCommandQueue_));
	assert(SUCCEEDED(result_));
}

void DirectXWrapper::InitializeSwapchain()
{
	//スワップチェーン設定
	swapChainDesc_.Width = (uint32_t)WindowsApp::GetInstance().WINDOW_WIDTH_;
	swapChainDesc_.Height = (uint32_t)WindowsApp::GetInstance().WINDOW_HEIGHT_;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc_.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc_.BufferCount = 2; // バッファ数を2つに設定
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// スワップチェーンの生成
	ComPtr<IDXGISwapChain1> swapChain1;

	result_ = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), WindowsApp::GetInstance().Gethwnd(), &swapChainDesc_, nullptr, nullptr,
		&swapChain1);
	assert(SUCCEEDED(result_));

	swapChain1.As(&swapChain_);
}

void DirectXWrapper::InitializeRendertargetView()
{
	//デスクリプタヒープの設定
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount; // 裏表の2つ
	// デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(rtvHeap_.GetAddressOf()));

	//バックバッファ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//RTV生成
		// スワップチェーンの全てのバッファについて処理する
	for (int32_t i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		swapChain_->GetBuffer((uint32_t)i, IID_PPV_ARGS(&backBuffers_[i]));
		// デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}


}

void DirectXWrapper::InitializeDepthBuffer()
{
	//06_01
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = (uint16_t)WindowsApp::GetInstance().WINDOW_WIDTH_;//レンダーターゲットに合わせる
	depthResourceDesc.Height = (uint32_t)WindowsApp::GetInstance().WINDOW_HEIGHT_;//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;//深度値フォーマット(バッファーとしてのビット数は32だがビット数以外の扱いは最終的に決められる)
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
	result_ = device_->CreateCommittedResource(
		&depthHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
	assert(SUCCEEDED(result_));
	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは一つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	result_ = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap_.GetAddressOf()));
	assert(SUCCEEDED(result_));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,//深度ビュー
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()//ヒープの先頭に作る
	);

	//---------

	//深度値をテクスチャとして使うためのSRVのヒープ作成
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result_ = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&depthSRVHeap_));
	assert(SUCCEEDED(result_));

	//srv作成
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	device_->CreateShaderResourceView(depthBuff_.Get(), &resDesc,
		depthSRVHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXWrapper::InitializeFence()
{
	//フェンス生成
	result_ = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

	result_ = device_->CreateFence(texFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&texFence_));
}

void DirectXWrapper::InitializeFixFPS()
{
	//現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::UpdateFixFPS()
{
	//1/60(s)ぴったりの時間
	const std::chrono::microseconds MIN_TIME(uint64_t(1000000.0f / 60.0f));
	//1/60(s)よりわずかに短い時間
	const std::chrono::microseconds MIN_CHECK_TIME(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60(s)（よりわずかに短い時間）経ってない場合
	if (elapsed < MIN_CHECK_TIME)
	{
		//1/60(s)経過するまで微小なスリープ繰り返す
		while (std::chrono::steady_clock::now() - reference_ < MIN_CHECK_TIME)//
		{
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::CreateCommandLA(ID3D12CommandAllocator** texCommandAllocator, ID3D12GraphicsCommandList** texCommandList)
{
	// コマンドアロケータを生成
	result_ = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(texCommandAllocator));
	assert(SUCCEEDED(result_));
	// コマンドリストを生成
	result_ = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		(*texCommandAllocator), nullptr,
		IID_PPV_ARGS(texCommandList));
	assert(SUCCEEDED(result_));
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
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf()))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);//やばいエラー時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);//エラー時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);//警告時止まる
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
	static DirectXWrapper sInst; // private なコンストラクタを呼び出す。
	return sInst;
}

void DirectXWrapper::CommandReset()
{
	// キューをクリア
	result_ = commandAllocator_->Reset();
	assert(SUCCEEDED(result_));
	// 再びコマンドリストを貯める準備
	result_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result_));
}

void DirectXWrapper::DrawInitialize()
{

}

void DirectXWrapper::UpLoadTexture()
{
	// 命令のクローズ
#pragma region CmdClose
	HRESULT result = texCommandList_->Close();
	assert(SUCCEEDED(result));
	// 溜めていたコマンドリストの実行(close必須)
	ID3D12CommandList* commandLists[] = { texCommandList_.Get() };
	texCommandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion CmdClose

#pragma region ChangeScreen
	// コマンドの実行完了を待つ
	texCommandQueue_->Signal(texFence_.Get(), ++texFenceVal_);
	if (texFence_->GetCompletedValue() != texFenceVal_)	//	GPUの処理が完了したか判定
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		texFence_->SetEventOnCompletion(texFenceVal_, event);
		if (event != 0) {
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
	// キューをクリア
	result = texCommandAllocator_->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = texCommandList_->Reset(texCommandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
	texUploadBuff_.clear();
}

//実際に描画のpre
void DirectXWrapper::PreDraw()
{
	// バックバッファの番号を取得(2つなので0番か1番)
	uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();

	rtvHandle_ = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	// レンダーターゲットビューのハンドルを取得
	rtvHandle_.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	//1.リソースバリアで書き込み可能に変更
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get(); // バックバッファを指定
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	commandList_->ResourceBarrier(1, &barrierDesc_);

	//レンダーターゲット(ポストペラの一枚目でdsvと紐づけるのでここではなし)
	commandList_->OMSetRenderTargets(
		1, &rtvHandle_, false, nullptr
	);

	// 3.画面クリア R G B A
	commandList_->ClearRenderTargetView(rtvHandle_, clearColor_, 0, nullptr);

	// ビューポート設定コマンドを、コマンドリストに積む
	commandList_->RSSetViewports(1, &WindowsApp::GetInstance().viewport_);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (long)(scissorRect.left + WindowsApp::GetInstance().WINDOW_WIDTH_); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (long)(scissorRect.top + WindowsApp::GetInstance().WINDOW_HEIGHT_); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXWrapper::PostDraw()
{
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
	commandList_->ResourceBarrier(1, &barrierDesc_);

	// 命令のクローズ
	result_ = commandList_->Close();
	assert(SUCCEEDED(result_));
	// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// 画面に表示するバッファをフリップ(裏表の入替え)
	result_ = swapChain_->Present(1, 0);

	// コマンドの実行完了を待つ
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)//GPUの処理が完了したか
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		if (event != 0) {
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}

	//FPS固定
	UpdateFixFPS();

	//リセット
	CommandReset();

	assert(SUCCEEDED(result_));
}

void DirectXWrapper::ResourceBarrier(D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState, ID3D12Resource* buff)
{
	D3D12_RESOURCE_BARRIER barrierDesc{};

	barrierDesc.Transition.pResource = buff;
	barrierDesc.Transition.StateBefore = beforeState; // 描画状態から
	barrierDesc.Transition.StateAfter = afterState; // 表示状態へ
	commandList_->ResourceBarrier(1, &barrierDesc);
}

void LoadPictureFromFile(const char* fileName, ComPtr<ID3D12Resource>& texBuff)
{
	HRESULT result = {};

	wchar_t fileNameL[128];
	ConstCharToWcharT(fileName, fileNameL);

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	result = LoadFromWICFile(
		fileNameL,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//mipmap生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

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
		const Image* IMG = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = IMG->pixels;
		subResourcesDatas[i].RowPitch = IMG->rowPitch;
		subResourcesDatas[i].SlicePitch = IMG->slicePitch;
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
void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, uint32_t size)
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
	HRESULT result = {};

	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(result));
}


