#include "PostPera.h"
#include <d3dx12.h>
#include "ImGuiManager.h"


const float PostPera::S_BLOOM_POW_ = 9.0f;
const float  PostPera::S_BLOOM_MAX_POW_ = 15.0f;
const float  PostPera::S_RADIAL_MAX_POW_ = 1.0f;


void PostPera::Initialize(const char* normalImageFileName)
{
	HRESULT result = {};

	//rtvなど作る
	InitializeBuffRTV(normalImageFileName);

	//画面効果用定数バッファ
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(EffectConstBuffer) + DirectXWrapper::S_BUFFER_ALIGNMENT_) & ~DirectXWrapper::S_BUFFER_ALIGNMENT_/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//定数バッファのマッピング
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}

	//頂点バッファ
	CreateVertexBuff(p1Vertices_, _countof(p1Vertices_), pera1VB_.GetAddressOf(), &pera1VBV_);
	CreateVertexBuff(pGVertices_, _countof(pGVertices_), peraGVB_.GetAddressOf(), &peraGVBV_);
	CreateVertexBuff(p2Vertices_, _countof(p2Vertices_), pera2VB_.GetAddressOf(), &pera2VBV_);

	//ルートシグネチャ、パイプライン
	GenerateRSPL();
}

void PostPera::CreateVertexBuff(PeraVertex* vertex, int32_t vertexCount, ID3D12Resource** vB, D3D12_VERTEX_BUFFER_VIEW* vBV)
{
	//頂点バッファ
	uint32_t sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<uint32_t>(sizeof(vertex[0]) * vertexCount);
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, vB);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vBV->BufferLocation = (*vB)->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vBV->SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vBV->StrideInBytes = sizeof(vertex[0]);
}

void PostPera::MappingVertexBuff(PeraVertex* vertex, int32_t vertexCount, float peraExtend, ID3D12Resource* vB)
{
	//マップ
// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	PeraVertex* vertMap = nullptr;
	HRESULT result = S_OK;
	result = vB->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int32_t i = 0; i < vertexCount; i++) {
		vertMap[i] =
		{
			{vertex[i].pos.x * peraExtend ,vertex[i].pos.y * peraExtend ,vertex[i].pos.z  },
			{vertex[i].uv.x,vertex[i].uv.y}
		}; // 座標をコピー
	}

	vB->Unmap(0, nullptr);
}

//directxのバックバッファなど生成した後に呼び出す
void PostPera::InitializeBuffRTV(const char* normalImageFileName)
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

	//ブルーム(一枚目の三、1.5枚目の四、五つ目)
	CreateBloomBuffer(resDesc, heapProp, clearValue);

	//被写界深度用
	CreateDofBuffer(resDesc, heapProp, clearValue);

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

	//----------------------------------------------------------------------------------

	// RTV 用 ヒープ を 作る 
	heapDesc.NumDescriptors = 7;//3枚分(一枚目の3つ(通常、違う色、高輝度、）+ 1.5枚目の3つ（ぼかし、ぼかし２、 被写界深度用) + 二枚目の一つ)
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

	//(1.5枚目の三,四,五つ目)ブルーム
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

	//被写界深度用
	DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
		dofBuffer_.Get(),
		&rtvDesc,
		handleRH
	);
	//インクリメント
	handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//二枚目
	DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
		peraResource2_.Get(),
		&rtvDesc,
		handleRH
	);

	//---------------------------------------------------------------------------------------

	//SRV用ヒープを作る
	heapDesc.NumDescriptors = 10;//一枚目の3つ(通常、違う色、高輝度、）+ 1.5枚目の3つ（ぼかし、ぼかし２、通常ぼかし（被写界深度用）)
	//+二枚目の一つ,ガウシアンパラメータ、ガラス、深度値用で10枚
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

	//ブルーム（1枚目(の高輝度)3つ目、1.5枚目の4,5つ目）
	CreateBloomBuffView();

	//被写界深度用(6つ目)
	//一つ前でハンドルインクリメントしてる
	CreateDofSRV();

	//インクリメント
	peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//二枚目(7つ目)
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
		peraResource2_.Get(),
		&srvDesc,
		peraSRHandle_
	);

	//ガウシアンパラメータ(8つ目)
	gaussianBuff_.Initialize(peraSRHandle_, *DirectXWrapper::GetInstance().GetDevice(), heapDesc);

	//ガラスフィルター(9つ目)
	GlassFilterBuffGenerate(normalImageFileName);

	//深度値テクスチャ用SRV(10つ目)
	CreateDepthTexBuffAndSRV();
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
	D3D12_DESCRIPTOR_RANGE range[RootParamIndex::COUNT] = {};
	//一枚目の1つ目(通常カラー)
	range[RootParamIndex::COLOR].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::COLOR].BaseShaderRegister = 0;//0
	range[RootParamIndex::COLOR].NumDescriptors = 1;
	//一枚目の2つ目(違うカラー)
	range[RootParamIndex::COLOR2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::COLOR2].BaseShaderRegister = 1;//1
	range[RootParamIndex::COLOR2].NumDescriptors = 1;
	//一枚目の3つ目(高輝度)
	range[RootParamIndex::HIGH_LUMI].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::HIGH_LUMI].BaseShaderRegister = 2;//2
	range[RootParamIndex::HIGH_LUMI].NumDescriptors = 1;
	//1.5枚目の1つ目(縮小)
	range[RootParamIndex::SHRINK].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::SHRINK].BaseShaderRegister = 3;//3
	range[RootParamIndex::SHRINK].NumDescriptors = 1;
	//1.5枚目の2つ目(縮小2)
	range[RootParamIndex::SHRINK2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::SHRINK2].BaseShaderRegister = 4;//4
	range[RootParamIndex::SHRINK2].NumDescriptors = 1;
	//1.5枚目の3つ目（被写界深度用
	range[RootParamIndex::DEPTH_FIELD].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::DEPTH_FIELD].BaseShaderRegister = 5;//5
	range[RootParamIndex::DEPTH_FIELD].NumDescriptors = 1;
	//ガウシアン定数
	range[RootParamIndex::GAUSS_PARAM].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[RootParamIndex::GAUSS_PARAM].BaseShaderRegister = 1;//フラグを送るのがb0なので1
	range[RootParamIndex::GAUSS_PARAM].NumDescriptors = 1;
	//ガラス
	range[RootParamIndex::GLASS].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::GLASS].BaseShaderRegister = 6;//6
	range[RootParamIndex::GLASS].NumDescriptors = 1;
	//深度値テクスチャ用
	range[RootParamIndex::DEPTH_TEX].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[RootParamIndex::DEPTH_TEX].BaseShaderRegister = 7;//7
	range[RootParamIndex::DEPTH_TEX].NumDescriptors = 1;


	//実際に使うルートパラメータ
	D3D12_ROOT_PARAMETER rp[RootParamIndex::COUNT] = {};
	//SRV(一枚目の1つ目(通常カラー))
	rp[RootParamIndex::COLOR].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::COLOR].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::COLOR].DescriptorTable.pDescriptorRanges = &range[COLOR];
	rp[RootParamIndex::COLOR].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(一枚目の2つ目(違うカラー))
	rp[RootParamIndex::COLOR2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::COLOR2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::COLOR2].DescriptorTable.pDescriptorRanges = &range[COLOR2];
	rp[RootParamIndex::COLOR2].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(一枚目の3つ目(高輝度))
	rp[RootParamIndex::HIGH_LUMI].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::HIGH_LUMI].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::HIGH_LUMI].DescriptorTable.pDescriptorRanges = &range[HIGH_LUMI];
	rp[RootParamIndex::HIGH_LUMI].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(1.5枚目の4つ目(縮小))
	rp[RootParamIndex::SHRINK].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::SHRINK].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::SHRINK].DescriptorTable.pDescriptorRanges = &range[SHRINK];
	rp[RootParamIndex::SHRINK].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(1.5枚目の5つ目(縮小2))
	rp[RootParamIndex::SHRINK2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::SHRINK2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::SHRINK2].DescriptorTable.pDescriptorRanges = &range[SHRINK2];
	rp[RootParamIndex::SHRINK2].DescriptorTable.NumDescriptorRanges = 1;
	//SRV(6つ目(被写界深度用))
	rp[RootParamIndex::DEPTH_FIELD].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::DEPTH_FIELD].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::DEPTH_FIELD].DescriptorTable.pDescriptorRanges = &range[DEPTH_FIELD];
	rp[RootParamIndex::DEPTH_FIELD].DescriptorTable.NumDescriptorRanges = 1;
	//ボケ定数用
	rp[RootParamIndex::GAUSS_PARAM].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::GAUSS_PARAM].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::GAUSS_PARAM].DescriptorTable.pDescriptorRanges = &range[GAUSS_PARAM];
	rp[RootParamIndex::GAUSS_PARAM].DescriptorTable.NumDescriptorRanges = 1;
	//ポストエフェクトのフラグ
	rp[RootParamIndex::EFFECT_FLAG].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rp[RootParamIndex::EFFECT_FLAG].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rp[RootParamIndex::EFFECT_FLAG].Descriptor.RegisterSpace = 0;//デフォルト値
	rp[RootParamIndex::EFFECT_FLAG].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//ガラスフィルタ
	rp[RootParamIndex::GLASS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::GLASS].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::GLASS].DescriptorTable.pDescriptorRanges = &range[GLASS];
	rp[RootParamIndex::GLASS].DescriptorTable.NumDescriptorRanges = 1;
	//深度値テクスチャ
	rp[RootParamIndex::DEPTH_TEX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[RootParamIndex::DEPTH_TEX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[RootParamIndex::DEPTH_TEX].DescriptorTable.pDescriptorRanges = &range[DEPTH_TEX];
	rp[RootParamIndex::DEPTH_TEX].DescriptorTable.NumDescriptorRanges = 1;


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


	//----------------------------------------------------
	//（1.5枚目）ぼかし用
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
	//高輝度＆高輝度2＆通常（被写界深度用）
	//ぼかし用パイプライン生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(blurPipeline_.ReleaseAndGetAddressOf())
	);
}


bool PostPera::CreateBloomBuffer(const D3D12_RESOURCE_DESC& resDesc,
	const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue)
{
	auto resDescL = resDesc;

	//バッファー作成
	int32_t count = 0;
	for (auto& res : bloomBuffers_)
	{
		HRESULT result = S_OK;
		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDescL,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
		//ここでバッファのサイズ縮小(高輝度ぼかし、〃２だけサイズ小さくなる)
		if (count == 0)
		{
			resDescL.Width >>= 1;
		}
		count++;
	}

	return true;
}

bool PostPera::CreateBloomBuffView()
{
	//ビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//３枚作る（高輝度、ぼかし、ぼかし２）
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

bool PostPera::CreateDofBuffer(const D3D12_RESOURCE_DESC& resDesc,
	const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue)
{
	D3D12_RESOURCE_DESC resDescL = resDesc;

	//縮小バッファなので大きさ半分
	resDescL.Width >>= 1;
	//バッファー作成
	HRESULT result = S_OK;
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDescL,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(dofBuffer_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));
	return true;
}

bool PostPera::CreateDofSRV()
{
	//ビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//作成
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(dofBuffer_.Get(), &srvDesc,
		peraSRHandle_);

	return true;
}

void PostPera::GlassFilterBuffGenerate(const char* fileName)
{
	//ガラスフィルター（9つ目）
	//インクリメント
	peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CreateEffectBufferAndView(fileName);
}

bool PostPera::CreateEffectBufferAndView(const char* fileName)
{
	////法線マップをロード（SRVも作成）
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	TextureManager::GetInstance().LoadGraph(fileName, &effectTexBuffer_, &srvDesc, &peraSRHandle_);

	return true;
}

bool PostPera::CreateDepthTexBuffAndSRV()
{
	//インクリメント
	peraSRHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//srv作成
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	//深度バッファと紐づける
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(DirectXWrapper::GetInstance().GetDepthBuff(), &resDesc,
		peraSRHandle_);

	return true;
}

void PostPera::Update()
{
	effectFlags_.time++;
	*mapEffectFlagsBuff_ = effectFlags_;
	//強さで色を割るので小さい方が強くなる
	mapEffectFlagsBuff_->bloomPow = (S_BLOOM_MAX_POW_ + 1.0f - effectFlags_.bloomPow);
}

void PostPera::ImGuiUpdate(int32_t num)
{
	std::string name = "PostEffect" + std::to_string(num);

	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::SliderInt("MultiTex", (int*)&effectFlags_.isMultiTex, 0, 1);
		ImGui::SliderInt("GrayScale", (int*)&effectFlags_.isGrayScale, 0, 1);
		ImGui::SliderFloat("GrayScalePow", &effectFlags_.grayScalePow, 0, 1.0f);
		ImGui::SliderInt("Emboss", (int*)&effectFlags_.isEmboss, 0, 1);
		ImGui::SliderInt("Gaussian", (int*)&effectFlags_.isGaussian, 0, 1);
		ImGui::SliderInt("Gaussian2", (int*)&effectFlags_.isGaussian2, 0, 1);
		ImGui::SliderInt("RadialBlur", (int*)&effectFlags_.isRadialBlur, 0, 1);
		ImGui::SliderFloat("RadialPow", &effectFlags_.radialPow, 0.0f, S_RADIAL_MAX_POW_);
		ImGui::SliderInt("GlassFilter", (int*)&effectFlags_.isGlassFilter, 0, 1);
		ImGui::SliderInt("ScanningLine", (int*)&effectFlags_.isScanningLine, 0, 1);
		ImGui::SliderInt("BarrelCurve", (int*)&effectFlags_.isBarrelCurve, 0, 1);
		ImGui::SliderFloat("BarrelCurvePow", &effectFlags_.barrelCurvePow, 0, 1.0f);
		ImGui::SliderInt("Gradation", (int*)&effectFlags_.isGradation, 0, 1);
		ImGui::SliderInt("Outline", (int*)&effectFlags_.isOutLine, 0, 1);
		ImGui::SliderInt("Sharpness", (int*)&effectFlags_.isSharpness, 0, 1);
		ImGui::SliderInt("Vignette", (int*)&effectFlags_.isVignette, 0, 1);
		ImGui::SliderInt("Mosaic", (int*)&effectFlags_.isMosaic, 0, 1);
		ImGui::SliderInt("NegaPosi", (int*)&effectFlags_.isNega, 0, 1);
		ImGui::SliderInt("RGBShift", (int*)&effectFlags_.isRGBShift, 0, 1);
		ImGui::SliderFloat("RGBShiftPow", &effectFlags_.RGBShiftPow, 0.0f, 0.05f);
		ImGui::SliderInt("Bloom", (int*)&effectFlags_.isBloom, 0, 1);
		ImGui::SliderInt("CrossFilter", (int*)&effectFlags_.isCrossFilter, 0, 1);
		ImGui::SliderFloat("BloomPow", &effectFlags_.bloomPow, 1.0f, S_BLOOM_MAX_POW_);
		ImGui::SliderInt("Noise", (int*)&effectFlags_.isNoise, 0, 1);
		ImGui::SliderFloat("NoisePow", &effectFlags_.noisePow, 0, 1.0f);
		ImGui::SliderInt("DepthField", (int*)&effectFlags_.isDepthField, 0, 1);
		ImGui::SliderFloat("FocusDepth", &effectFlags_.focusDepth, 0, 2.0f);
		ImGui::SliderFloat("NFocusWidth", &effectFlags_.nFocusWidth, 0, 2.0f);
		ImGui::SliderFloat("focusDiffPow", &effectFlags_.focusDiffPow, 0, 2.0f);
		ImGui::SliderInt("Sea", (int*)&effectFlags_.isSea, 0, 1);
		ImGui::SliderInt("SeaImageEffect", (int*)&effectFlags_.isSeaImageEffect, 0, 1);
		ImGui::DragFloat3("SeaCameraPos", &effectFlags_.seaCameraPos.x);
		ImGui::DragFloat3("SeaDirRot", &effectFlags_.seaDirRot.x, 0.0001f);
		ImGui::DragFloat("Resoulution", &effectFlags_.resoulution, 0.05f);
		ImGui::DragFloat3("seaDirRotExt", &effectFlags_.seaDirRotExtend.x, 0.01f);
		ImGui::DragFloat3("seaColor", &effectFlags_.seaCol.x, 0.05f);
		ImGui::ColorEdit3("skyColor", &effectFlags_.seaSkyCol.x);
		ImGui::DragFloat("seaTimeExtend", &effectFlags_.seaTimerExtend, 0.05f);
		ImGui::ColorEdit4("color", &effectFlags_.color.x);

		ImGui::TreePop();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
//一枚目を描画
void PostPera::Draw()
{
	//マップ
	MappingVertexBuff(p1Vertices_, _countof(p1Vertices_), pera1Extend_, pera1VB_.Get());

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(peraRS_.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(peraPipeline_.Get());

	//ヒープをセットしたり、テクスチャセットしたり
	SetHeapAllBuffView(false);

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &pera1VBV_);

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

//1.5枚目
void PostPera::DrawShrinkTextureForBlur()
{
	//ブルームなどぼかす処理をしないなら
	if (!effectFlags_.isBloom && !effectFlags_.isCrossFilter && !effectFlags_.isDepthField)
	{
		return;
	}

	//マップ
	MappingVertexBuff(pGVertices_, _countof(pGVertices_), peraGExtend_, peraGVB_.Get());

	ID3D12GraphicsCommandList* cmdList =
		DirectXWrapper::GetInstance().GetInstance().GetCommandList();

	D3D12_RESOURCE_BARRIER barrierDesc{};

	//縮小バッファ,〃２はレンダーターゲットに([1],[2])
	for (int32_t i = 1; i < 3; i++)
	{
		DirectXWrapper::GetInstance().ResourceBarrier(
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			bloomBuffers_[i].Get());
	}
	//被写界深度用
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		dofBuffer_.Get());

	//rtvハンドル
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvs[3] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE baseHandle = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	auto incSize = DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//ぼかし、ぼかし２、通常ぼかし（被写界深度用）は4,5,6つ目
	uint32_t offset = DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 3;
	for (auto& handle : rtvs)
	{
		handle.InitOffsetted(baseHandle, offset);
		offset += incSize;
	}
	//レンダーターゲットセット
	//ぼかし二つ+通常ぼかし（被写界深度用）
	cmdList->OMSetRenderTargets(3, rtvs, false, nullptr);

	{
		//パイプラインセット
		cmdList->SetPipelineState(blurPipeline_.Get());
		//シグネイチャーセット
		cmdList->SetGraphicsRootSignature(peraRS_.Get());
		//デスクリプタヒープ
		SetHeapAllBuffView();

		//頂点バッファセット
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		cmdList->IASetVertexBuffers(0, 1, &peraGVBV_);

		{
			//高輝度を縮小
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

	//縮小バッファ,〃２はシェーダーリソースに
	for (int32_t i = 1; i < 3; i++)
	{
		DirectXWrapper::GetInstance().ResourceBarrier(
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			bloomBuffers_[i].Get());
	}
	//被写界深度用
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		dofBuffer_.Get());

}

//実際に描画
void PostPera::Draw2()
{
	//マップ
	MappingVertexBuff(p2Vertices_, _countof(p2Vertices_), pera2Extend_, pera2VB_.Get());

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(peraRS_.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(peraPipeline2_.Get());
	//ヒープをセット
	SetHeapAllBuffView(true);

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &pera2VBV_);

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//一枚目のテクスチャに描画
void PostPera::PreDraw(bool isFirstPostPera)
{
	//ポストエフェクト
	for (ComPtr< ID3D12Resource>& res : peraResources_)
	{
		DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			res.Get());
	}

	//ブルーム一つ目（高輝度）
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		bloomBuffers_[0].Get());


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


	if (!isFirstPostPera)
	{
		//レンダーターゲットをセット
		DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(3, rtvs, false, nullptr);
	}
	//一番最初のポストペラだったら深度
	else
	{
		//レンダーターゲットをセット
		DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(3, rtvs, false, &dsvHandle);
	}

	// 3.画面クリア R G B A
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	for (auto& rt : rtvs)
	{
		DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rt, clsClr, 0, nullptr);
	}

	//一番最初のポストペラだったら深度
	if (isFirstPostPera)
	{
		//06_01(dsv)
		DirectXWrapper::GetInstance().GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
			1.0f, 0, 0, nullptr);
		//毎フレーム深度バッファの値が描画範囲で最も奥(1.0)にリセットされる
	}

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
		DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			res.Get());
	}

	//ブルームの一つ目（高輝度）
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		bloomBuffers_[0].Get());
}

//-----------------------------
//一枚目を二枚目に描画
void PostPera::PreDraw2()
{
	//状態をレンダーターゲットに遷移
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		peraResource2_.Get());

	// 一枚目の二つ目（色違い）
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		peraResources_[1].Get());

	// // 高輝度を指定
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		bloomBuffers_[0].Get());


	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[3] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE baseH = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	//二枚目（7つ目なので）
	baseH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 6;
	rtvHandle[0] = baseH;
	//（一枚目の2つ目)なので進める
	baseH = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	baseH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHandle[1] = baseH;
	//（高輝度は3つ目)なので進める
	baseH = peraRTVHeap_->GetCPUDescriptorHandleForHeapStart();
	baseH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 2;
	rtvHandle[2] = baseH;
	//レンダーターゲットセット
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(
		3, rtvHandle, false, nullptr
	);


	// 3.画面クリア R G B A
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	for (auto& rt : rtvHandle)
	{
		DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rt, clsClr, 0, nullptr);
	}

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

void PostPera::PostDraw2()
{
	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		peraResource2_.Get());

	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		peraResources_[1].Get());

	DirectXWrapper::GetInstance().ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		bloomBuffers_[0].Get());

}


//---------------------------------------------------------------------
//一枚目に描画(引数の描画関数を一枚目に描画)
void PostPera::DrawToPostpera(const std::function<void()>& f, bool isFirstPostPera, const std::function<void()>& spriteDrawF)
{
	//一枚目
	{
		PreDraw(isFirstPostPera);

		//ペラにオブジェクトなど描画させる
		if (f)
		{
			f();
		}

		//スプライト
		if (spriteDrawF)
		{
			spriteDrawF();
		}

		PostDraw();
	}
}

//ブラーの描画をまとめた
void PostPera::DrawToBlurAll()
{
	PreDraw2();

	Draw();

	PostDraw2();
}

//ヒープとヒープ上に作ったバッファビュー等をセット
void PostPera::SetHeapAllBuffView(bool isPost2, bool isPost1)
{
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, peraSRVHeap_.GetAddressOf());

	//一枚目の3つ(通常、違う色、高輝度、）+ 1.5枚目の3つ（ぼかし、ぼかし２、通常ぼかし（被写界深度用）)
	//+二枚目の一つ,ガウシアンパラメータ、ガラス用で9枚
	auto peraHandle = peraSRVHeap_->GetGPUDescriptorHandleForHeapStart();
	//パラメーターとヒープを関連付ける(一枚目の3つ(通常カラー、違うカラー、高輝度))
	//+ 1.5枚目の3つ（ぼかし、ぼかし２、通常ぼかし（被写界深度用）
	for (uint32_t i = 0; i < RootParamIndex::DEPTH_FIELD + 1; i++)
	{
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(i, peraHandle);
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//二枚目
	if (isPost2)
	{
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(RootParamIndex::COLOR, peraHandle);//SRVなのでrp[0](とりあえず一枚目)
	}

	//ボケ定数
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(RootParamIndex::GAUSS_PARAM, peraHandle);

	//ポストエフェクトの「フラグ」なのでrp[6]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(RootParamIndex::EFFECT_FLAG, effectFlagsBuff_->GetGPUVirtualAddress());

	//ガラスフィルター
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(RootParamIndex::GLASS, peraHandle);

	if (!isPost1)
	{
		//深度値テクスチャ
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(RootParamIndex::DEPTH_TEX, peraHandle);
	}
}
