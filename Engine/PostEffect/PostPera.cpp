#include "PostPera.h"
#include <d3dx12.h>
#include "ImGuiManager.h"


void PostPera::Initialize(const wchar_t* nomalImageFileName)
{
	//rtvなど作る
	InitializeBuffRTV();

	//ガラスフィルター
	GlassFilterBuffGenerate(nomalImageFileName);

	//定数バッファ0番(画面効果フラグ)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	//画面効果用
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff);
		//定数バッファのマッピング
		DirectXWrapper::GetInstance().result = effectFlagsBuff->Map(0, nullptr, (void**)&mapEffectFlagsBuff);//マッピング
		assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	}


	//バッファ
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_peraVB.ReleaseAndGetAddressOf())
	);

	//ビュー
	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	//マップ
	PeraVertex* mappedPera = nullptr;
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	std::copy(std::begin(pv), std::end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	GenerateRSPL();
}

//directxのバックバッファなど生成した後に呼び出す
void PostPera::InitializeBuffRTV()
{
	// 作成 済み の ヒープ 情報 を 使っ て もう 1 枚 作る
	auto heapDesc = DirectXWrapper::GetInstance().GetRtvheap()->GetDesc();
	// 使っ て いる バックバッファー の 情報 を 利用 する 
	auto& bbuff = DirectXWrapper::GetInstance().GetBackBuffer()[0];
	auto resDesc = bbuff->GetDesc();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//レンダリング時のクリア地と同じ値
	float clsClr[4] = { DirectXWrapper::GetInstance().GetClearColor()[0],DirectXWrapper::GetInstance().GetClearColor()[1],
		DirectXWrapper::GetInstance().GetClearColor()[2],DirectXWrapper::GetInstance().GetClearColor()[3] };
	D3D12_CLEAR_VALUE clearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clsClr);

	HRESULT result;

	//リソース作成
	//一枚目
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			//D3D12_RESOURCE_STATE_RENDER_TARGETではない
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
	}

	//二枚目
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		//D3D12_RESOURCE_STATE_RENDER_TARGETではない
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(_peraResource2.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	// RTV 用 ヒープ を 作る 
	heapDesc.NumDescriptors = 3;//二枚分(一枚目の二つ+二枚目の一つ)
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_peraRTVHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//rtvを作る
	D3D12_CPU_DESCRIPTOR_HANDLE handleRH = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	//1枚目
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
			res.Get(),
			&rtvDesc,
			handleRH
		);
		//インクリメント
		handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//二枚目
	//handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
		_peraResource2.Get(),
		&rtvDesc,
		handleRH
	);


	//SRV用ヒープを作る
	heapDesc.NumDescriptors = 5;//ガウシアンパラメータ、一枚目の二つ、二枚目の一つ,ガラス用で4枚
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_peraSRVHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// シェーダーリソースビュー（ SRV） を 作る 
	peraHandle = _peraSRVHeap->GetCPUDescriptorHandleForHeapStart();
	//1枚目
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
			res.Get(),
			&srvDesc,
			peraHandle
		);

		//インクリメント
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//二枚目(3つ目)
	//インクリメント
	//peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
		_peraResource2.Get(),
		&srvDesc,
		peraHandle
	);

	//ガウシアンパラメータ(4つ目)
	gausianBuff.Initialize(peraHandle, *DirectXWrapper::GetInstance().GetDevice(), heapDesc);
}

void PostPera::GenerateRSPL()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.InputLayout.NumElements = _countof(layout);
	gpsDesc.InputLayout.pInputElementDescs = layout;


	//pipeline
	//頂点
	HRESULT result = D3DCompileFromFile(
		L"Resources/shaders/PeraVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS",
		"vs_5_0",
		0,
		0,
		vs.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));
	//ピクセル
	result = D3DCompileFromFile(
		L"Resources/shaders/PeraPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS",
		"ps_5_0",
		0,
		0,
		ps.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	gpsDesc.VS.pShaderBytecode = vs->GetBufferPointer();
	gpsDesc.VS.BytecodeLength = vs->GetBufferSize();
	gpsDesc.PS.pShaderBytecode = ps->GetBufferPointer();
	gpsDesc.PS.BytecodeLength = ps->GetBufferSize();

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 2;//peraResource1は二つレンダーターゲット
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(rootParams);//ルートパラメータ数;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootsig

	D3D12_DESCRIPTOR_RANGE range[3] = {};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[0].BaseShaderRegister = 0;//0
	range[0].NumDescriptors = 1;
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[1].BaseShaderRegister = 1;//フラグを送るのがb0なので1
	range[1].NumDescriptors = 1;
	//ガラス
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[2].BaseShaderRegister = 1;//1
	range[2].NumDescriptors = 1;

	//実際に使うルートパラメータ
	D3D12_ROOT_PARAMETER rp[4] = {};
	//SRV
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	//ボケ定数用
	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	rp[2] = rootParams[0];//フラグのやつ
	//ガラスフィルタ
	rp[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[3].DescriptorTable.pDescriptorRanges = &range[2];
	rp[3].DescriptorTable.NumDescriptorRanges = 1;


	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0); //s0

	//デスクリプタテーブル+フラグのやつ
	rsDesc.NumParameters = _countof(rp);
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;


	ComPtr<ID3DBlob>rsBlob;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rsBlob.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(
		0,
		rsBlob.Get()->GetBufferPointer(),
		rsBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(_peraRS.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//パイプラインステート
	//一枚目
	gpsDesc.pRootSignature = _peraRS.Get();
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline.GetAddressOf())
	);
	assert(SUCCEEDED(result));


	//二枚目
	result = D3DCompileFromFile(
		L"Resources/shaders/PeraPixelShader2.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS2",
		"ps_5_0",
		0,
		0,
		ps.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	//vs等は同じのを使って、psのみ二枚目
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
	//2枚目用パイプライン生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline2.ReleaseAndGetAddressOf())
	);
}


void PostPera::GlassFilterBuffGenerate(const wchar_t* fileName)
{
	//ガラスフィルター（4つ目）
	//インクリメント
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	this->CreateEffectBufferAndView(fileName);
}

bool PostPera::CreateEffectBufferAndView(const wchar_t* fileName)
{
	//法線マップをロード
	LoadPictureFromFile(fileName, this->_effectTexBuffer);

	//ポストエフェクト用テクスチャビューを作る
	auto desc = _effectTexBuffer->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(_effectTexBuffer.Get(), &srvDesc, this->peraHandle);

	return true;
}

void PostPera::Update()
{
	ImGui::Begin("PostEffect");
	ImGui::SliderInt("GrayScale", (int*)&effectFlags.isGrayScale, 0, 1);
	ImGui::SliderInt("Emboss", (int*)&effectFlags.isEmboss, 0, 1);
	ImGui::SliderInt("Gaussian", (int*)&effectFlags.isGaussian, 0, 1);
	ImGui::SliderInt("GlassFilter", (int*)&effectFlags.isGlassFilter, 0, 1);
	ImGui::SliderInt("ScanningLine", (int*)&effectFlags.isScanningLine, 0, 1);
	ImGui::SliderInt("BarrelCurve", (int*)&effectFlags.isBarrelCurve, 0, 1);
	ImGui::SliderInt("Gradation", (int*)&effectFlags.isGradation, 0, 1);
	ImGui::SliderInt("Outline", (int*)&effectFlags.isOutLine, 0, 1);
	ImGui::SliderInt("Sharpness", (int*)&effectFlags.isSharpness, 0, 1);
	ImGui::SliderInt("Vignette", (int*)&effectFlags.isVignette, 0, 1);
	ImGui::SliderInt("Mosaic", (int*)&effectFlags.isMosaic, 0, 1);
	ImGui::SliderInt("NegaPosi", (int*)&effectFlags.isNega, 0, 1);
	ImGui::SliderInt("RGBShift", (int*)&effectFlags.isRGBShift, 0, 1);
	ImGui::End();

	this->mapEffectFlagsBuff->isEmboss = effectFlags.isEmboss;
	this->mapEffectFlagsBuff->isGaussian = effectFlags.isGaussian;
	this->mapEffectFlagsBuff->isGaussian2 = effectFlags.isGaussian2;
	this->mapEffectFlagsBuff->isGradation = effectFlags.isGradation;
	this->mapEffectFlagsBuff->isOutLine = effectFlags.isOutLine;
	this->mapEffectFlagsBuff->isSharpness = effectFlags.isSharpness;
	this->mapEffectFlagsBuff->isVignette = effectFlags.isVignette;
	this->mapEffectFlagsBuff->isBarrelCurve = effectFlags.isBarrelCurve;
	this->mapEffectFlagsBuff->isScanningLine = effectFlags.isScanningLine;
	this->mapEffectFlagsBuff->isGrayScale = effectFlags.isGrayScale;
	this->mapEffectFlagsBuff->isGlassFilter = effectFlags.isGlassFilter;
	this->mapEffectFlagsBuff->isMosaic = effectFlags.isMosaic;
	this->mapEffectFlagsBuff->isNega = effectFlags.isNega;
	this->mapEffectFlagsBuff->isRGBShift = effectFlags.isRGBShift;
	this->mapEffectFlagsBuff->time++;
}

//----------------------------------------------------------------------------------------------------------------------------------
//一枚目を描画
void PostPera::Draw()
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, _peraSRVHeap.GetAddressOf());

	auto peraHandle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	//パラメーター0番とヒープを関連付ける
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);

	//ボケ定数(一枚目の二つと二枚目の一つを飛ばす)
	for (int i = 0; i < 3; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, peraHandle);

	//ガラスフィルター
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(3, peraHandle);

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//ポストエフェクトフラグなのでrp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

//実際に描画
void PostPera::Draw2()
{
	//DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline2.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, _peraSRVHeap.GetAddressOf());

	auto peraHandle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();

	//二枚目
	for (int i = 0; i < 2; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);//SRVなのでrp[0]

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//ポストエフェクトフラグなのでrp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//一枚目のテクスチャに描画
void PostPera::PreDraw()
{
	//ポストエフェクト
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		barrierDesc.Transition.pResource = res.Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
			1,
			&barrierDesc
		);
	}

	// 2.描画先の変更
	// 1 パス 目 
	//auto rtvHeapPointer = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXWrapper::GetInstance().GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	/*DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(
		1, &rtvHeapPointer, false, &dsvHandle
	);*/


	auto rtvHeapPointer = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapPointer.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvs[2] = {
		_peraRTVHeap->GetCPUDescriptorHandleForHeapStart(),
		rtvHeapPointer
	};
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(2, rtvs, false, &dsvHandle);


	// 3.画面クリア R G B A
	//DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rtvHeapPointer, clearColor, 0, nullptr);
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	for (auto& rt : rtvs)
	{
		DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rt, clsClr, 0, nullptr);
	}

	//06_01(dsv)
	DirectXWrapper::GetInstance().GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	//毎フレーム深度バッファの値が描画範囲で最も奥(1.0)にリセットされる

	// ビューポート設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostPera::PostDraw()
{
	// 5.リソースバリアを戻す
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		barrierDesc.Transition.pResource = res.Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 表示状態へ
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
	}
}

//-----------------------------
//二枚目に描画
void PostPera::PreDraw2()
{
	//状態をレンダーターゲットに遷移
	barrierDesc.Transition.pResource = _peraResource2.Get(); // 二枚目のリソースを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 表示状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
		1,
		&barrierDesc
	);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	//3個目(一枚目の二つの後)なので進める
	rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2 パス 目 
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXWrapper::GetInstance().GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(
		1, &rtvHandle, false, &dsvHandle
	);


	// 3.画面クリア R G B A
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rtvHandle, clsClr, 0, nullptr);
	//06_01(dsv)
	DirectXWrapper::GetInstance().GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	//毎フレーム深度バッファの値が描画範囲で最も奥(1.0)にリセットされる

	// ビューポート設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostPera::PostDraw2()
{
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 表示状態へ
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
}


//---------------------------------------------------------------------
//二枚目の描画をまとめた
void PostPera::Draw2All()
{
	PreDraw2();

	Draw();

	PostDraw2();
}

//一枚目と二枚目を描画する(引数の描画関数を一枚目に描画)
void PostPera::DrawToPostpera(std::function<void()> f)
{
	//一枚目
	{
		PreDraw();

		//ペラにオブジェクトなど描画させる
		f();

		PostDraw();
	}

	//------------
	//二枚目
	{
		Draw2All();
	}
}