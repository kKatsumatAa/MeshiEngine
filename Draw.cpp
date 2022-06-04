#include "Draw.h"

//設定をもとにSRV用デスクリプタヒープを生成
ID3D12DescriptorHeap* srvHeap = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

Draw::Draw(const WindowsApp& win, Directx& directx):
	directx(directx),win(win)
{
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用
	
	ResourceProperties(resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, &vertBuff);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);


	// 頂点シェーダの読み込みとコンパイル
	directx.result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	Error(FAILED(directx.result));

	// ピクセルシェーダの読み込みとコンパイル
	directx.result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	Error(FAILED(directx.result));

	//04_02
	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;      //一度の描画に使うテクスチャの枚数
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//ルートパラメータの設定
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1;//定数バッファ番号(b1)
	rootParams[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える


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
	directx.result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(directx.result));
	constBuffTransfer({ 1.0f,0,0,1.0f });

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
	directx.result = indexBuff->Map(0, nullptr, (void**)&indexMap);
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

	//04_02
	////横方向ピクセル数
	//const size_t textureWidth = 256;
	////縦方向ピクセル数
	//const size_t textureHeight = 256;
	////配列の要素数
	//const size_t imageDataCount = textureWidth * textureHeight;
	////画像イメージデータ配列
	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];//あとで必ず解放！！
	////全ピクセルの色を初期化
	//for (size_t i = 0; i < imageDataCount; i++)
	//{
	//	imageData[i].x = 1.0f;//R
	//	imageData[i].y = 0.0f;//G
	//	imageData[i].z = 0.0f;//B
	//	imageData[i].w = 1.0f;//A
	//}

	


	//05_02
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		ResourceProperties(cbResourceDesc, (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffTransform);
	}
	//定数バッファのマッピング
	directx.result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//マッピング
	assert(SUCCEEDED(directx.result));
	//単位行列を代入
	//SetNormDigitalMat(constMapTransform->mat, win);

	////05_03
	////平行投影変換（スプライト描画?）
	//constMapTransform->mat =
	//	XMMatrixOrthographicOffCenterLH(0.0, win.window_width, win.window_height, 0.0, 0.0f, 1.0f);


	////05_04
	//ViewMat view ( 0.0f, 0.0f, -100.0f
	//	, 0.0f, 0.0f, 0.0f
	//	, 0.f, 1.f, 0.f );

	////定数バッファに転送
	//constMapTransform->mat = view.matView * matProjection;

	//05_05
	/*XMMATRIX matWorld;
	matWorld = XMMatrixIdentity();

	constMapTransform=matWorld**/
}

void Draw::Update( unsigned short* indices, const int& pipelineNum,
	const bool& primitiveMode)
{
	//constBuffTransfer({ -0.001f,0.001f,0,0 });

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	directx.result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(directx.result));
	// 全頂点に対して
	for (int i = 0; i < _countof(this->vertices); i++) {
		vertMap[i] = this->vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// ビューポート設定コマンド
	viewport = { 0, 0, win.window_width, win.window_height, 0.0f, 1.0f };
	// ビューポート設定コマンドを、コマンドリストに積む
	directx.commandList->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = scissorRect.left + win.window_width; // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + win.window_height; // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	directx.commandList->RSSetScissorRects(1, &scissorRect);

	// パイプラインステートとルートシグネチャの設定コマンド
	directx.commandList->SetPipelineState(pipelineState[pipelineNum]);
	directx.commandList->SetGraphicsRootSignature(rootSignature);

	// プリミティブ形状の設定コマンド
	directx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角か四角

	// 頂点バッファビューの設定コマンド
	directx.commandList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBV)の設定コマンド
	directx.commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//04_02
	//SRVヒープの設定コマンド
	directx.commandList->SetDescriptorHeaps(1, &srvHeap);
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	directx.commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//定数バッファビュー(CBV)の設定コマンド
	directx.commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	//インデックスバッファビューの設定コマンド
	directx.commandList->IASetIndexBuffer(&ibView);

	// 描画コマンド
	directx.commandList->DrawIndexedInstanced(sizeof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画
}

void Draw::DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, const int& pipelineNum)
{
	vertices[0] = { pos1,{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{0.5f,0.0f} };//上
	vertices[2] = { pos3,{1.0f,1.0f} };//右下
	vertices[3] = vertices[1];//右上
	
	Update(indices, pipelineNum);
}

void Draw::DrawBox(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, const int& pipelineNum)
{
	vertices[0] = { pos1,{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{0.0f,0.0f} };//左上
	vertices[2] = { pos3,{1.0f,1.0f} };//右下
	vertices[3] = { pos4,{1.0f,0.0f} };//右上

	Update(indices2, pipelineNum);
}
void Draw::DrawBoxSprite(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, const int& pipelineNum)
{
	vertices[0] = { pos1,{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{0.0f,0.0f} };//左上
	vertices[2] = { pos3,{1.0f,1.0f} };//右下
	vertices[3] = { pos4,{1.0f,0.0f} };//右上

	//05_03
	//平行投影変換（スプライト描画?）
	constMapTransform->mat =
		XMMatrixOrthographicOffCenterLH(0.0, win.window_width, win.window_height, 0.0, 0.0f, 1.0f);

	Update(indices2, pipelineNum);
}

void Draw::LoadGraph(const wchar_t* name)
{
	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	directx.result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(directx.result));

	ScratchImage mipChain{};
	//mipmap生成
	directx.result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(directx.result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	////リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	//textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//textureResourceDesc.Width = textureWidth;						//頂点データ全体のサイズ
	//textureResourceDesc.Height = textureHeight;
	//textureResourceDesc.DepthOrArraySize = 1;
	//textureResourceDesc.MipLevels = 1;
	//textureResourceDesc.SampleDesc.Count = 1;
	// 04_03
	
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;						//頂点データ全体のサイズ
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	//テクスチャバッファの生成
	ID3D12Resource* texBuff = nullptr;
	BuffProperties(textureHeapProp, textureResourceDesc, &texBuff);
	////テクスチャバッファにデータ転送
	//directx.result = texBuff->WriteToSubresource(
	//	0,
	//	nullptr,//全領域へコピー
	//	imageData,//元データアドレス
	//	sizeof(XMFLOAT4) * textureWidth,//1ラインサイズ
	//	sizeof(XMFLOAT4) * imageDataCount//全サイズ
	//);

	//04_03
	// 全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		directx.result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//全サイズ
		);
		assert(SUCCEEDED(directx.result));
	}
	////元データ解放
	//delete[] imageData;

	
	
	//設定をもとにSRV用デスクリプタヒープを生成
	if(srvHandle.ptr == 0) directx.result = directx.device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(directx.result));
	//SRVヒープの先頭ハンドルを取得
	if (srvHandle.ptr == 0)srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	else srvHandle.ptr += directx.device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

	//シェーダーリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//srvDesc.Shader4ComponentMapping =
	//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = 1;
	////ハンドルのさす位置にシェーダーリソースビュー作成
	//device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	//04_03
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;
	//ハンドルのさす位置にシェーダーリソースビュー作成
	directx.device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	
}

void Draw::PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState)
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

	//04_02
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //奥行き繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                  //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                               //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	directx.result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(directx.result));
	directx.result = directx.device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(directx.result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	directx.result = directx.device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	assert(SUCCEEDED(directx.result));
}

void Draw::Blend(const D3D12_BLEND_OP& blendMode, const bool& Inversion, const bool& Translucent)
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

void Draw::constBuffTransfer(const XMFLOAT4& plusRGBA)
{
	color2.x += plusRGBA.x;
	color2.y += plusRGBA.y;
	color2.z += plusRGBA.z;
	color2.w += plusRGBA.w;

	//値を書き込むと自動的に転送される
	constMapMaterial->color = color2;//半透明の赤
}

void Draw::ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void Draw::BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	directx.result = directx.device->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(directx.result));
}

void Draw::SetNormDigitalMat(XMMATRIX& mat, const WindowsApp& win)
{
	mat.r[0].m128_f32[0] = 2.0f / win.window_width;
	mat.r[1].m128_f32[1] = -2.0f / win.window_height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Draw::Error(const bool& filed)
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

