#include "PostPera.h"
#include <d3dx12.h>
#include "ImGuiManager.h"


void PostPera::Initialize(const wchar_t* nomalImageFileName)
{
	HRESULT result = {};

	//rtvなど作る
	InitializeBuffRTV(nomalImageFileName);

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
			((uint32_t)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//定数バッファのマッピング
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}

	//頂点バッファ
	uint32_t sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<uint32_t>(sizeof(pVertices_[0]) * _countof(pVertices_));
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, peraVB_.GetAddressOf());

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	peraVBV_.BufferLocation = peraVB_.Get()->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	peraVBV_.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	peraVBV_.StrideInBytes = sizeof(pVertices_[0]);

	//マップ
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	PeraVertex* vertMap = nullptr;
	result = peraVB_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int32_t i = 0; i < _countof(pVertices_); i++) {
		vertMap[i] = pVertices_[i]; // 座標をコピー
	}
	// 繋がりを解除
	peraVB_->Unmap(0, nullptr);

	//ルートシグネチャ、パイプライン
	GenerateRSPL();
}

//directxのバックバッファなど生成した後に呼び出す
void PostPera::InitializeBuffRTV(const wchar_t* P_NORMAL_IMAGE_FILE_NAME)
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
	for (ComPtr< ID3D12Resource>& res : peraResources_)
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

	//ブルーム(一枚目の三、四つ目)
	CreateBloomBuffer();

	//二枚目
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		//D3D12_RESOURCE_STATE_RENDER_TARGETではない
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(peraResource2_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	// RTV 用 ヒープ を 作る 
	heapDesc.NumDescriptors = 5;//二枚分(一枚目の4つ(通常、違う色、高輝度,ぼかし)+二枚目の一つ)
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(peraRTVHeap_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//rtvを作る
	D3D12_CPU_DESCRIPTOR_HANDLE handleRH = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	//1枚目(1,2つ)
	for (ComPtr< ID3D12Resource>& res : peraResources_)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
			res.Get(),
			&rtvDesc,
			handleRH
		);
		//インクリメント
		handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//(一枚目の三,四つ目)ブルーム
	for (ComPtr< ID3D12Resource>& res : bloomBuffers_)
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
	DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
		peraResource2_.Get(),
		&rtvDesc,
		handleRH
	);


	//SRV用ヒープを作る
	heapDesc.NumDescriptors = 7;//一枚目の4つ(通常、違う色、高輝度、ぼかし)、二枚目の一つ,ガウシアンパラメータ、ガラス用で7枚
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(peraSRVHeap_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// シェーダーリソースビュー（ SRV） を 作る 
	peraSRHandle_ = peraSRVHeap_->GetCPUDescriptorHandleForHeapStart();
	//1枚目（二つ作る)
	for (ComPtr< ID3D12Resource>& res : peraResources_)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
			res.Get(),
			&srvDesc,
			peraSRHandle_
		);
		//インクリメント
		peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//ブルーム（(一枚目の)3、4つ目）
	CreateBloomBuffView();

	//二枚目(5つ目)
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
		peraResource2_.Get(),
		&srvDesc,
		peraSRHandle_
	);

	//ガウシアンパラメータ(6つ目)
	gausianBuff_ = std::make_unique<GausianBuffer>();
	gausianBuff_->Initialize(peraSRHandle_, *DirectXWrapper::GetInstance().GetDevice(), heapDesc);

	//ガラスフィルター(7つ目)
	GlassFilterBuffGenerate(P_NORMAL_IMAGE_FILE_NAME);
}

void PostPera::GenerateRSPL()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
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
		vs_.ReleaseAndGetAddressOf(),
		errBlob_.ReleaseAndGetAddressOf()
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
		ps_.ReleaseAndGetAddressOf(),
		errBlob_.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	gpsDesc.VS.pShaderBytecode = vs_->GetBufferPointer();
	gpsDesc.VS.BytecodeLength = vs_->GetBufferSize();
	gpsDesc.PS.pShaderBytecode = ps_->GetBufferPointer();
	gpsDesc.PS.BytecodeLength = ps_->GetBufferSize();

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 3;//peraResource1は3つレンダーターゲット
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumStaticSamplers = 0;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootsig

	D3D12_DESCRIPTOR_RANGE range[6] = {};
	//一枚目の1つ目(通常カラー)
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[0].BaseShaderRegister = 0;//0
	range[0].NumDescriptors = 1;
	//一枚目の2つ目(違うカラー)
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[1].BaseShaderRegister = 1;//1
	range[1].NumDescriptors = 1;
	//一枚目の3つ目(高輝度)
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[2].BaseShaderRegister = 2;//2
	range[2].NumDescriptors = 1;
	//一枚目の4つ目(縮小)
	range[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[3].BaseShaderRegister = 3;//3
	range[3].NumDescriptors = 1;
	//ガウシアン定数
	range[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[4].BaseShaderRegister = 1;//フラグを送るのがb0なので1
	range[4].NumDescriptors = 1;
	//ガラス
	range[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[5].BaseShaderRegister = 4;//4
	range[5].NumDescriptors = 1;


	//実際に使うルートパラメータ
	D3D12_ROOT_PARAMETER rp[7] = {};
	//SRV(一枚目の1つ目(通常カラー))
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(一枚目の2つ目(違うカラー))
	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(一枚目の3つ目(高輝度))
	rp[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[2].DescriptorTable.pDescriptorRanges = &range[2];
	rp[2].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(一枚目の4つ目(縮小))
	rp[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[3].DescriptorTable.pDescriptorRanges = &range[3];
	rp[3].DescriptorTable.NumDescriptorRanges = 1;
	//ボケ定数用
	rp[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[4].DescriptorTable.pDescriptorRanges = &range[4];
	rp[4].DescriptorTable.NumDescriptorRanges = 1;
	//ポストエフェクトのフラグ
	rp[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rp[5].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rp[5].Descriptor.RegisterSpace = 0;//デフォルト値
	rp[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//ガラスフィルタ
	rp[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[6].DescriptorTable.pDescriptorRanges = &range[5];
	rp[6].DescriptorTable.NumDescriptorRanges = 1;


	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); //s0
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//タイリングすると反対側からとってきたりしておかしくなるので

	//デスクリプタテーブル+ポストエフェクトのフラグ+ガラス
	rsDesc.NumParameters = _countof(rp);
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;


	ComPtr<ID3DBlob>rsBlob;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rsBlob.ReleaseAndGetAddressOf(),
		errBlob_.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(
		0,
		rsBlob.Get()->GetBufferPointer(),
		rsBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(peraRS_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//パイプラインステート
	//一枚目
	gpsDesc.pRootSignature = peraRS_.Get();
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(peraPipeline_.GetAddressOf())
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
		ps_.ReleaseAndGetAddressOf(),
		errBlob_.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	//vs等は同じのを使って、psのみ二枚目
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps_.Get());
	//2枚目用パイプライン生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(peraPipeline2_.ReleaseAndGetAddressOf())
	);


	//（三枚目？）ぼかし用
	result = D3DCompileFromFile(
		L"Resources/shaders/BlurPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BlurPS",
		"ps_5_0",
		0,
		0,
		ps_.ReleaseAndGetAddressOf(),
		errBlob_.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	//vs等は同じのを使って、psのみぼかし用
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps_.Get());
	//ぼかし用パイプライン生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(blurPipeline_.ReleaseAndGetAddressOf())
	);
}


void PostPera::GlassFilterBuffGenerate(const wchar_t* P_FILE_NAME)
{
	//ガラスフィルター（7つ目）
	//インクリメント
	peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CreateEffectBufferAndView(P_FILE_NAME);
}

bool PostPera::CreateEffectBufferAndView(const wchar_t* P_FILE_NAME)
{
	//法線マップをロード
	LoadPictureFromFile(P_FILE_NAME, effectTexBuffer_);

	//ポストエフェクト用テクスチャビューを作る
	auto desc = effectTexBuffer_->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(effectTexBuffer_.Get(), &srvDesc, peraSRHandle_);

	return true;
}

bool PostPera::CreateBloomBuffer()
{
	// 使っ て いる バックバッファー の 情報 を 利用 する 
	auto& bbuff = DirectXWrapper::GetInstance().GetBackBuffer()[0];
	auto resDesc = bbuff->GetDesc();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//レンダリング時のクリア地と同じ値
	float clsClr[4] = { DirectXWrapper::GetInstance().GetClearColor()[0],DirectXWrapper::GetInstance().GetClearColor()[1],
		DirectXWrapper::GetInstance().GetClearColor()[2],DirectXWrapper::GetInstance().GetClearColor()[3] };
	D3D12_CLEAR_VALUE clearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clsClr);

	//バッファー作成
	for (auto& res : bloomBuffers_)
	{
		auto result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
		);
		resDesc.Width >>= 1;
	}

	return true;
}

bool PostPera::CreateBloomBuffView()
{
	//ビュー作成
	//インクリメント
	//peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//二枚作る
	for (ComPtr< ID3D12Resource>& res : bloomBuffers_)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
			res.Get(),
			&srvDesc,
			peraSRHandle_);

		//インクリメント
		peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	return true;
}

void PostPera::Update()
{
	ImGui::Begin("PostEffect");
	ImGui::SliderInt("GrayScale", (int*)&effectFlags_.isGrayScale, 0, 1);
	ImGui::SliderInt("Emboss", (int*)&effectFlags_.isEmboss, 0, 1);
	ImGui::SliderInt("Gaussian", (int*)&effectFlags_.isGaussian, 0, 1);
	ImGui::SliderInt("GlassFilter", (int*)&effectFlags_.isGlassFilter, 0, 1);
	ImGui::SliderInt("ScanningLine", (int*)&effectFlags_.isScanningLine, 0, 1);
	ImGui::SliderInt("BarrelCurve", (int*)&effectFlags_.isBarrelCurve, 0, 1);
	ImGui::SliderInt("Gradation", (int*)&effectFlags_.isGradation, 0, 1);
	ImGui::SliderInt("Outline", (int*)&effectFlags_.isOutLine, 0, 1);
	ImGui::SliderInt("Sharpness", (int*)&effectFlags_.isSharpness, 0, 1);
	ImGui::SliderInt("Vignette", (int*)&effectFlags_.isVignette, 0, 1);
	ImGui::SliderInt("Mosaic", (int*)&effectFlags_.isMosaic, 0, 1);
	ImGui::SliderInt("NegaPosi", (int*)&effectFlags_.isNega, 0, 1);
	ImGui::SliderInt("RGBShift", (int*)&effectFlags_.isRGBShift, 0, 1);
	ImGui::SliderInt("Bloom", (int*)&effectFlags_.isBloom, 0, 1);
	ImGui::End();

	mapEffectFlagsBuff_->isEmboss = effectFlags_.isEmboss;
	mapEffectFlagsBuff_->isGaussian = effectFlags_.isGaussian;
	mapEffectFlagsBuff_->isGaussian2 = effectFlags_.isGaussian2;
	mapEffectFlagsBuff_->isGradation = effectFlags_.isGradation;
	mapEffectFlagsBuff_->isOutLine = effectFlags_.isOutLine;
	mapEffectFlagsBuff_->isSharpness = effectFlags_.isSharpness;
	mapEffectFlagsBuff_->isVignette = effectFlags_.isVignette;
	mapEffectFlagsBuff_->isBarrelCurve = effectFlags_.isBarrelCurve;
	mapEffectFlagsBuff_->isScanningLine = effectFlags_.isScanningLine;
	mapEffectFlagsBuff_->isGrayScale = effectFlags_.isGrayScale;
	mapEffectFlagsBuff_->isGlassFilter = effectFlags_.isGlassFilter;
	mapEffectFlagsBuff_->isMosaic = effectFlags_.isMosaic;
	mapEffectFlagsBuff_->isNega = effectFlags_.isNega;
	mapEffectFlagsBuff_->isRGBShift = effectFlags_.isRGBShift;
	mapEffectFlagsBuff_->isBloom = effectFlags_.isBloom;
	mapEffectFlagsBuff_->time++;
}

//----------------------------------------------------------------------------------------------------------------------------------
//一枚目を描画
void PostPera::Draw()
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(peraRS_.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(peraPipeline_.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, peraSRVHeap_.GetAddressOf());

	auto peraHandle = peraSRVHeap_->GetGPUDescriptorHandleForHeapStart();
	//パラメーター0番とヒープを関連付ける(一枚目の3つ(通常カラー、違うカラー、高輝度))
	for (uint32_t i = 0; i < 3; i++)
	{
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(i, peraHandle);
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//一枚目の四つ目（ぼかし高輝度）、二枚目飛ばす
	for (uint32_t i = 0; i < 2; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//ボケ定数
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(4, peraHandle);

	//ポストエフェクトの「フラグ」なのでrp[5]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff_->GetGPUVirtualAddress());

	//ガラスフィルター
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(6, peraHandle);

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &peraVBV_);

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

//1.5枚目
void PostPera::DrawShrinkTextureForBlur()
{
	ID3D12GraphicsCommandList* cmdList =
		DirectXWrapper::GetInstance().GetInstance().GetCommandList();

	D3D12_RESOURCE_BARRIER barrierDesc{};

	//縮小バッファはレンダーターゲットに
	barrierDesc.Transition.pResource = bloomBuffers_[1].Get(); // バックバッファを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソースから
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // レンダーターゲットへ
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
		1,
		&barrierDesc
	);

	//rtvハンドル
	auto rtvHandle = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットは一枚目の四つ目
	rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 3;
	//レンダーターゲットセット
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	{
		//パイプラインセット
		cmdList->SetPipelineState(blurPipeline_.Get());
		//シグネイチャーセット
		cmdList->SetGraphicsRootSignature(peraRS_.Get());
		//デスクリプタヒープ
		cmdList->SetDescriptorHeaps(1, peraSRVHeap_.GetAddressOf());

		//srvハンドル
		auto srvHandle = peraSRVHeap_->GetGPUDescriptorHandleForHeapStart();

		//テクスチャは一枚目の三つ目
		for (uint32_t i = 0; i < 2; i++)
		{
			srvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
				GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		//1パス目高輝度をテクスチャとして使用
		cmdList->SetGraphicsRootDescriptorTable(2, srvHandle);
		srvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//
		cmdList->SetGraphicsRootDescriptorTable(3, srvHandle);

		//頂点バッファセット
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		cmdList->IASetVertexBuffers(0, 1, &peraVBV_);

		{
			auto desc = bloomBuffers_[0]->GetDesc();
			D3D12_VIEWPORT vp = {};
			D3D12_RECT sr = {};
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;
			vp.Height = desc.Height / 2.0f;
			vp.Width = desc.Width / 2.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			sr.top = 0;
			sr.left = 0;
			sr.right = (long)vp.Width;
			sr.bottom = (long)vp.Height;

			for (int32_t i = 0; i < 8; ++i) {
				cmdList->RSSetViewports(1, &vp);
				cmdList->RSSetScissorRects(1, &sr);
				cmdList->DrawInstanced(4, 1, 0, 0);
				//書いたら下にずらして次を書く準備
				sr.top += (long)vp.Height;
				vp.TopLeftX = 0;
				vp.TopLeftY = (float)sr.top;
				//幅も高さも半分に
				vp.Width /= 2.0f;
				vp.Height /= 2.0f;
				sr.bottom += (long)vp.Height;
			}
		}
	}

	//縮小バッファをシェーダーリソースに
	barrierDesc.Transition.pResource = bloomBuffers_[1].Get(); // バックバッファを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // レンダーターゲットから
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソースへ 
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
		1,
		&barrierDesc
	);

}

//実際に描画
void PostPera::Draw2()
{
	//DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(peraPipeline2_.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, peraSRVHeap_.GetAddressOf());

	auto peraHandle = peraSRVHeap_->GetGPUDescriptorHandleForHeapStart();

	//（1枚目の3つ飛ばす(通常カラー、違うカラー、ブルーム用,別だが（ぼかし高輝度）)）
	for (int32_t i = 0; i < 3; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	//高輝度ぼかし
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(3, peraHandle);


	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//二枚目
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);//SRVなのでrp[0](とりあえず一枚目)

	//ポストエフェクトフラグなのでrp[5]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff_->GetGPUVirtualAddress());

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &peraVBV_);

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//一枚目のテクスチャに描画
void PostPera::PreDraw()
{
	//ポストエフェクト
	for (ComPtr< ID3D12Resource>& res : peraResources_)
	{
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = res.Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソースから
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
			1,
			&barrierDesc
		);
	}

	//ブルーム
	//for (ComPtr< ID3D12Resource>& res : _bloomBuffer)
	{
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = bloomBuffers_[0].Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソースから
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
			1,
			&barrierDesc
		);
	}

	// 2.描画先の変更
	// 1 パス 目 
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXWrapper::GetInstance().GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();

	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	//一枚目の3つ(通常カラー、違うカラー、ブルーム用)
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvs[3] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE baseHandle = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	auto incSize = DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	uint32_t offset = 0;
	for (auto& handle : rtvs)
	{
		handle.InitOffsetted(baseHandle, offset);
		offset += incSize;
	}

	//レンダーターゲットをセット
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(3, rtvs, false, &dsvHandle);


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
	D3D12_VIEWPORT viewPorts[3];
	viewPorts[0] = { 0, 0, WindowsApp::GetInstance().WINDOW_WIDTH_, WindowsApp::GetInstance().WINDOW_HEIGHT_, 0.0f, 1.0f };
	viewPorts[1] = viewPorts[0];
	viewPorts[2] = viewPorts[0];

	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(3, viewPorts);


	CD3DX12_RECT scissorRects[3];
	scissorRects[0].left = 0; // 切り抜き座標左
	scissorRects[0].right = (long)(scissorRects[0].left + WindowsApp::GetInstance().WINDOW_WIDTH_); // 切り抜き座標右
	scissorRects[0].top = 0; // 切り抜き座標上
	scissorRects[0].bottom = (long)(scissorRects[0].top + WindowsApp::GetInstance().WINDOW_HEIGHT_); // 切り抜き座標下
	scissorRects[1] = scissorRects[0];
	scissorRects[2] = scissorRects[0];
	// シザー矩形設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(3, scissorRects);
}

void PostPera::PostDraw()
{
	// 5.リソースバリアを戻す
	for (ComPtr< ID3D12Resource>& res : peraResources_)
	{
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = res.Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソース
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
	}

	//ブルーム
	//for (ComPtr< ID3D12Resource>& res : _bloomBuffer)
	{
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = bloomBuffers_[0].Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // シェーダリソース
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
	}
}

//-----------------------------
//二枚目に描画
void PostPera::PreDraw2()
{
	//状態をレンダーターゲットに遷移
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = peraResource2_.Get(); // 二枚目のリソースを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 表示状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
		1,
		&barrierDesc
	);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	//3個目(一枚目の4つの後)なので進める
	for (uint32_t i = 0; i < 4; i++)
	{
		rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
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
	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(1, &WindowsApp::GetInstance().viewport_);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (long)(scissorRect.left + WindowsApp::GetInstance().WINDOW_WIDTH_); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (long)(scissorRect.top + WindowsApp::GetInstance().WINDOW_HEIGHT_); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostPera::PostDraw2()
{
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = peraResource2_.Get(); // 二枚目のリソースを指定
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
void PostPera::DrawToPostpera(const std::function<void()>& f)
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