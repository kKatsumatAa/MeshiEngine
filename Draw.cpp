#include "Draw.h"

//設定をもとにSRV用デスクリプタヒープを生成
ID3D12DescriptorHeap* srvHeap = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
int count = 0;

//SRVの最大個数
const size_t kMaxSRVCount = 2056;
//デスクリプタヒープの設定
static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {
srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
srvHeapDesc.NumDescriptors = kMaxSRVCount,
srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE//シェーダーから見えるように
};
//リソース設定
D3D12_RESOURCE_DESC resDesc{};

//インデックスバッファビューの作成
D3D12_INDEX_BUFFER_VIEW ibView{};
//インデックスバッファビューの作成
D3D12_INDEX_BUFFER_VIEW ibView2{};

static unsigned short indices[6] =
{
	0,1,2,//三角形1つ目
	2,1,3,//三角形2つ目
};
static unsigned short indices2[3] =
{
	0,1,2//三角形2つ目
};
static unsigned short indicesCube[36] =
{
	//前
	0,1,2,//三角形1つ目
	2,1,3,//三角形2つ目
	//奥
	6,5,4,//三角形1つ目
	7,5,6,//三角形2つ目
	//上
	10,9,8,//三角形1つ目
	11,9,10,//三角形2つ目
	//下
	12,13,14,//三角形1つ目
	14,13,15,//三角形2つ目
	//左
	16,17,18,//三角形1つ目
	18,17,19,//三角形2つ目
	//右
	22,21,20,//三角形1つ目
	23,21,22,//三角形2つ目
};
static unsigned short indicesCircle[] =
{
	0,1,2,
	0,2,3,
	0,3,4,
	0,4,5,
	0,5,6,
	0,6,7,
	0,7,8,
	0,8,9,
	0,9,10,
	0,10,11,
	0,11,12,
	0,12,13,
	0,13,14,
	0,14,15,
	0,15,16,
	0,16,17,
	0,17,18,
	0,18,19,
	0,19,20,
	0,20,21,
	0,21,22,
	0,22,23,
	0,23,1
};

static unsigned short indicesLine[2] =
{
	0,1//三角形2つ目
};

Vertex vertices[24] = {
	//手前
	{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
	{{-5.0f,5.0f, -5.0f},{},{0.0f,0.0f}},//左上
	{{5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	{{5.0f,5.0f,  -5.0f},{},{1.0f,0.0f}},//右上
	//奥
	{{-5.0f,-5.0f,5.0f},{},{0.0f,1.0f}},//左下
	{{-5.0f,5.0f, 5.0f},{},{0.0f,0.0f}},//左上
	{{5.0f,-5.0f, 5.0f},{},{1.0f,1.0f}},//右下
	{{5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//右上
	//上
	{{5.0f,5.0f,-5.0f},{},{0.0f,1.0f}},//左下
	{{5.0f,5.0f, 5.0f},{},{0.0f,0.0f}},//左上
	{{-5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	{{-5.0f,5.0f, 5.0f},{},{1.0f,0.0f}},//右上
	//下
	{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
	{{5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//左上
	{{-5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	{{-5.0f,-5.0f, 5.0f},{},{1.0f,0.0f}},//右上
	//左
	{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
	{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//左上
	{{-5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	{{-5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//右上
	//右
	{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
	{{5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//左上
	{{5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	{{5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//右上
};
//デスクリプタレンジの設定
D3D12_DESCRIPTOR_RANGE descriptorRange;
//テクスチャ

// 頂点レイアウト
D3D12_INPUT_ELEMENT_DESC inputLayout[3] = {
{//xyz座標
 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

	{//法線ベクトル
 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

{//uv座標
 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
} // (1行で書いたほうが見やすい)
};
//ルートパラメータの設定
D3D12_ROOT_PARAMETER rootParams[3] = {};
UINT sizeVB;

// ビューポート設定コマンド
D3D12_VIEWPORT viewport{};

// パイプランステートの生成
ID3D12PipelineState* pipelineState[2] = { nullptr };
// ルートシグネチャ
ID3D12RootSignature* rootSignature;
// グラフィックスパイプライン設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
ID3DBlob* errorBlob = nullptr; // エラーオブジェクト


//頂点バッファの設定
D3D12_HEAP_PROPERTIES heapProp{};
// 2.描画先の変更
	// レンダーターゲットビューのハンドルを取得
D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;


void DrawInitialize()
{
	// 頂点シェーダの読み込みとコンパイル
	Directx::GetInstance().result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	Error(FAILED(Directx::GetInstance().result));

	// ピクセルシェーダの読み込みとコンパイル
	Directx::GetInstance().result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	Error(FAILED(Directx::GetInstance().result));


	//デスクリプタレンジの設定
	descriptorRange.NumDescriptors = 100;   //一度の描画に使うテクスチャの枚数
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //テクスチャレジスタ0番(t0)
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

	

	//03_04
	//インデックスデータ
	//インデックスデータ全体のサイズ
	//int p= _countof(indicesCube);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesCircle));

	//リソース設定
	ResourceProperties(resDesc, sizeIB);
	//インデックスバッファの作成
	ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
	ID3D12Resource* indexBuff2 = nullptr;//GPU側のメモリ
	BuffProperties(heapProp, resDesc, &indexBuff);
	BuffProperties(heapProp, resDesc, &indexBuff2);
	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	uint16_t* indexMap2 = nullptr;
	Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	Directx::GetInstance().result = indexBuff2->Map(0, nullptr, (void**)&indexMap2);
	//全インデックスに対して
	for (int i = 0; i < _countof(indicesCube); i++)
	{
		indexMap[i] = indicesCube[i];//インデックスをコピー

	}
	for (int i = 0; i < _countof(indicesCircle); i++)
	{
		indexMap2[i] = indicesCircle[i];//インデックスをコピー
	}
	//マッピングを解除
	indexBuff->Unmap(0, nullptr);
	indexBuff2->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ibView2.BufferLocation = indexBuff2->GetGPUVirtualAddress();
	ibView2.Format = DXGI_FORMAT_R16_UINT;
	ibView2.SizeInBytes = sizeIB;
}

Draw::Draw()
{
	cbt.Initialize(Directx::GetInstance());
	

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

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
	Directx::GetInstance().result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(Directx::GetInstance().result));
	//constBuffTransfer({ 1.0f,1.0f,1.0f,1.0f });
	

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

	


	////05_02
	//{
	//	//ヒープ設定
	//	D3D12_HEAP_PROPERTIES cbHeapProp{};
	//	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//	//リソース設定
	//	D3D12_RESOURCE_DESC cbResourceDesc{};
	//	ResourceProperties(cbResourceDesc, (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256バイトアライメント*/);
	//	//定数バッファの生成
	//	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffTransform);
	//}
	////定数バッファのマッピング
	//Directx::GetInstance().result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//マッピング
	//assert(SUCCEEDED(Directx::GetInstance().result));
	//単位行列を代入
	//SetNormDigitalMat(constMapTransform->mat, win);

	////05_03
	////平行投影変換（スプライト描画?）
	//constMapTransform->mat =
	//	XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width, WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);


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
	constMapMaterial->color = { 0,0,0,1 };
}

void LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	Directx::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(Directx::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap生成
	Directx::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(Directx::GetInstance().result))
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
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
		&textureHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(Directx::GetInstance().result));

	////テクスチャバッファにデータ転送
	//Directx::GetInstance().result = texBuff->WriteToSubresource(
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
		Directx::GetInstance().result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//全サイズ
		);
		assert(SUCCEEDED(Directx::GetInstance().result));
	}
	////元データ解放
	//delete[] imageData;



	//設定をもとにSRV用デスクリプタヒープを生成
	if (count == 0)
	{                                                          //descは設定
		Directx::GetInstance().result = Directx::GetInstance().device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
		assert(SUCCEEDED(Directx::GetInstance().result));
	}
	//SRVヒープの先頭ハンドルを取得
	if (count == 0)srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	else srvHandle.ptr += Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

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
	srvDesc.Texture2D.MipLevels = 1;
	//ハンドルのさす位置にシェーダーリソースビュー作成
	Directx::GetInstance().device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	int count2 = count;
	count++;

	//04_02(画像貼る用のアドレスを引数に)
	//SRVヒープの設定コマンド
	Directx::GetInstance().commandList->SetDescriptorHeaps(1, &srvHeap);
	//SRVヒープのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count2);
}

void Draw::Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
	const bool& primitiveMode)
{
	//変換行列をGPUに送信
	if (worldMat->parent != nullptr)//親がいる場合
	{
		worldMat->matWorld *= worldMat->parent->matWorld;
		XMMATRIX matW;
		matW = { worldMat->matWorld.m[0][0],worldMat->matWorld.m[0][1],worldMat->matWorld.m[0][2],worldMat->matWorld.m[0][3],
				 worldMat->matWorld.m[1][0],worldMat->matWorld.m[1][1],worldMat->matWorld.m[1][2],worldMat->matWorld.m[1][3],
				 worldMat->matWorld.m[2][0],worldMat->matWorld.m[2][1],worldMat->matWorld.m[2][2],worldMat->matWorld.m[2][3],
				 worldMat->matWorld.m[3][0],worldMat->matWorld.m[3][1],worldMat->matWorld.m[3][2],worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}
	else//親がいない場合
	{
		XMMATRIX matW;
		matW = { worldMat->matWorld.m[0][0],worldMat->matWorld.m[0][1],worldMat->matWorld.m[0][2],worldMat->matWorld.m[0][3],
				 worldMat->matWorld.m[1][0],worldMat->matWorld.m[1][1],worldMat->matWorld.m[1][2],worldMat->matWorld.m[1][3],
				 worldMat->matWorld.m[2][0],worldMat->matWorld.m[2][1],worldMat->matWorld.m[2][2],worldMat->matWorld.m[2][3],
				 worldMat->matWorld.m[3][0],worldMat->matWorld.m[3][1],worldMat->matWorld.m[3][2],worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}

	//06_03
	for (int i = 0; i < _countof(vertices) / 3; i++)
	{//三角形一つごとに計算
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indicesCube[i * 3 + 0];
		unsigned short index1 = indicesCube[i * 3 + 1];
		unsigned short index2 = indicesCube[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//p0->p1ベクトル、p0->p2ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積（垂直なベクトル）
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(Directx::GetInstance().result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// ビューポート設定コマンド
	viewport = { 0, 0, WindowsApp::GetInstance().window_width, WindowsApp::GetInstance().window_height, 0.0f, 1.0f};
	// ビューポート設定コマンドを、コマンドリストに積む
	Directx::GetInstance().commandList->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = scissorRect.left + WindowsApp::GetInstance().window_width; // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + WindowsApp::GetInstance().window_height; // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	Directx::GetInstance().commandList->RSSetScissorRects(1, &scissorRect);

	// パイプラインステートとルートシグネチャの設定コマンド
	Directx::GetInstance().commandList->SetPipelineState(pipelineState[isWireFrame]);
	Directx::GetInstance().commandList->SetGraphicsRootSignature(rootSignature);

	// プリミティブ形状の設定コマンド
	if(primitiveMode)
	Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト
	else
	Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 線

	// 頂点バッファビューの設定コマンド
	Directx::GetInstance().commandList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBV)の設定コマンド
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//04_02
	if (textureHandle != NULL)
	{
		//SRVヒープの設定コマンド
		Directx::GetInstance().commandList->SetDescriptorHeaps(1, &srvHeap);
		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandle;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		Directx::GetInstance().commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}
	//定数バッファビュー(CBV)の設定コマンド
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

	//インデックスバッファビューの設定コマンド
	if(indexNum!=CIRCLE) Directx::GetInstance().commandList->IASetIndexBuffer(&ibView);
	else if (indexNum == CIRCLE)Directx::GetInstance().commandList->IASetIndexBuffer(&ibView2);

	int p = sizeof(indices);
	// 描画コマンド
	switch (indexNum)
	{
	case TRIANGLE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indices2), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case BOX:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case CUBE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesCube), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case LINE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesLine), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case CIRCLE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesCircle), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	}
}

void Draw::DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,
	WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0] = { pos1,{},{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{},{0.5f,0.0f} };//上
	vertices[2] = { pos3,{},{1.0f,1.0f} };//右下
	vertices[3] = vertices[1];//右上

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(TRIANGLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawBox(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, 
	WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0] = { pos1,{},{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{},{0.0f,0.0f} };//左上
	vertices[2] = { pos3,{},{1.0f,1.0f} };//右下
	vertices[3] = { pos4,{},{1.0f,0.0f} };//右上

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(BOX, pipelineNum, textureHandle,cbt);
}

void Draw::DrawBoxSprite(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, 
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	vertices[0] = { pos1,{},{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{},{0.0f,0.0f} };//左上
	vertices[2] = { pos3,{},{1.0f,1.0f} };//右下
	vertices[3] = { pos4,{},{1.0f,0.0f} };//右上

	/*if(color.x!=NULL&& color.y != NULL&& color.z != NULL&& color.w != NULL)*/ constMapMaterial->color = color;

	//05_03
	//平行投影変換（スプライト描画?）
	cbt.constMapTransform->mat =
		XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width, WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);

	Update(BOX, pipelineNum, textureHandle,cbt);
}

void Draw::DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	//手前
	vertices[0] = { {-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} };//左下
	vertices[1] = { {-5.0f,5.0f, -5.0f},{},{0.0f,0.0f} };//左上
	vertices[2] = { {5.0f,-5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	vertices[3] = { {5.0f,5.0f,  -5.0f},{},{1.0f,0.0f} };//右上

	vertices[4] = { {-5.0f,-5.0f,5.0f},{},{0.0f,1.0f} };//左下
	vertices[5] = { {-5.0f,5.0f, 5.0f},{},{0.0f,0.0f} };//左上
	vertices[6] = { {5.0f,-5.0f, 5.0f},{},{1.0f,1.0f} };//右下
	vertices[7] = { {5.0f,5.0f,  5.0f},{},{1.0f,0.0f} };//右上
		//上
	vertices[8] = { {5.0f,5.0f,-5.0f},{},{0.0f,1.0f} };//左下
	vertices[9] = { {5.0f,5.0f, 5.0f},{},{0.0f,0.0f} };//左上
	vertices[10] = { {-5.0f,5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	vertices[11] = { {-5.0f,5.0f, 5.0f},{},{1.0f,0.0f} };//右上

	vertices[12] = { {5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} };//左下
	vertices[13] = { {5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} };//左上
	vertices[14] = { {-5.0f,-5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	vertices[15] = { {-5.0f,-5.0f, 5.0f},{},{1.0f,0.0f} };//右上

	vertices[16] = { {-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} };//左下
	vertices[17] = { {-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} };//左上
	vertices[18] = { {-5.0f,5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	vertices[19] = { {-5.0f,5.0f,  5.0f},{},{1.0f,0.0f} };//右上

	vertices[20] = { {5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} };//左下
	vertices[21] = { {5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} };//左上
	vertices[22] = { {5.0f,5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	vertices[23] = { {5.0f,5.0f,  5.0f},{},{1.0f,0.0f} };//右上;//左下
	
	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(CUBE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawLine(XMFLOAT3 pos1, XMFLOAT3 pos2, WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4& color,
	const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	vertices[0] = { {pos1},{},{0.0f,1.0f} };
	vertices[1] = { {pos2},{},{0.0f,0.0f} };

	Update(LINE, pipelineNum, textureHandle, cbt, false);
}

void Draw::DrawCircle(float radius, WorldMat* world, ViewMat* view, ProjectionMat* projection, 
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0].pos = {0.0f,0.0f,0.0f} ;

	static float count = _countof(vertices) - 2;//中心点と初期の点はカウントしない

	for (int i = 1; i < _countof(vertices); i++)
	{
		vertices[i].pos = { radius * cosf(AngletoRadi(360 / count) * (i - 1)),radius * sinf(AngletoRadi(360 / count) * (i - 1)),0 };
	}
	//vertices[1] = { {-5.0f,5.0f, -5.0f},{},{0.0f,0.0f} };//左上
	//vertices[2] = { {5.0f,-5.0f, -5.0f},{},{1.0f,1.0f} };//右下
	//vertices[3] = { {5.0f,5.0f,  -5.0f},{},{1.0f,0.0f} };//右上

	constMapMaterial->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt);
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
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // カリングしない
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
	Directx::GetInstance().result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(Directx::GetInstance().result));
	Directx::GetInstance().result = Directx::GetInstance().device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(Directx::GetInstance().result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	//06_01
	//デプスステンシルステート
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	Directx::GetInstance().result = Directx::GetInstance().device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	//assert(SUCCEEDED(Directx::GetInstance().result));
}

void Blend(const D3D12_BLEND_OP& blendMode, const bool& Inversion, const bool& Translucent)
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
	if (constMapMaterial->color.x <= 1.0f && constMapMaterial->color.x >= 0.0f)
		constMapMaterial->color.x += plusRGBA.x;
	if (constMapMaterial->color.y <= 1.0f && constMapMaterial->color.y >= 0.0f)
		constMapMaterial->color.y += plusRGBA.y;
	if (constMapMaterial->color.z <= 1.0f && constMapMaterial->color.z >= 0.0f)
		constMapMaterial->color.z += plusRGBA.z;
	if (constMapMaterial->color.w <= 1.0f && constMapMaterial->color.w >= 0.0f)
		constMapMaterial->color.w += plusRGBA.w;
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

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(Directx::GetInstance().result));
}

void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().window_width;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().window_height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
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

