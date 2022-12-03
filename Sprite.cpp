#include "Sprite.h"

int count = 0;

void LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	assert(count <= srvCount - 1);

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
	//ID3D12Resource* texBuff = nullptr;
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&textureHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[count]));
	assert(SUCCEEDED(Directx::GetInstance().result));

	//04_03
	// 全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		Directx::GetInstance().result = texBuff[count]->WriteToSubresource(
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


	//SRVヒープの先頭ハンドルを取得
	if (count == 0)srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	else srvHandle.ptr += Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

	//04_03
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;
	//ハンドルのさす位置にシェーダーリソースビュー作成
	Directx::GetInstance().GetDevice()->CreateShaderResourceView(texBuff[count].Get(), &srvDesc, srvHandle);

	//04_02(画像貼る用のアドレスを引数に)
	//SRVヒープの設定コマンド
	Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//SRVヒープのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count);


	count++;
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
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(Directx::GetInstance().result));
}


void Sprite::Initialize()
{
	//sprite用
	{
		UINT sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB = static_cast<UINT>(sizeof(vertices[0]) * 4.0);
		//頂点バッファの設定		//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//頂点バッファの生成
		BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbView.BufferLocation = vertBuff.Get()->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbView.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbView.StrideInBytes = sizeof(vertices[0]);
	}
}

void SpriteCommonBeginDraw(PipeLineSet* pipelineSet)
{
	Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());

	Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());

	Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::SpriteDraw()
{
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	VertexSprite* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(Directx::GetInstance().result));
	// 全頂点に対して
	for (int i = 0; i < 4; i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	Directx::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Update(const Vec3& pos, const float& scale,
	XMFLOAT4 color, const UINT64 textureHandle, const Vec2& ancorUV,
	const bool& isReverseX,const bool& isReverseY, float rotation,
	ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//反転
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	vertices[0] = { {-(float)(length.x * scale * ancorUV.x),+(float)(length.y * scale * (1.0f - ancorUV.y)),0.0f},{0.0f,1.0f} };//左下
	vertices[1] = { {-(float)(length.x * scale * ancorUV.x),-(float)(length.y * scale * (ancorUV.y)),0.0f},{0.0f,0.0f} };//左上
	vertices[2] = { {+(float)(length.x * scale * (1.0f - ancorUV.x)),+(float)(length.y * scale * (1.0f - ancorUV.y)),0.0f},{1.0f,1.0f} };//右下
	vertices[3] = { {+(float)(length.x * scale * (1.0f - ancorUV.x)),-(float)(length.y * scale * (ancorUV.y)),0.0f},{1.0f,0.0f} };//右上


	constMapMaterial->color = color;

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);
	worldMat.trans = { pos.x /*+ length.x * ancorUV.x * scale*/,pos.y/* + length.y * ancorUV.y * scale*/,0.0f };
	worldMat.SetWorld();

	//親がいたら
	if (worldMat.parent != nullptr)
	{
		worldMat.matWorld *= worldMat.parent->matWorld;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld.m[0][0],(float)worldMat.matWorld.m[0][1],(float)worldMat.matWorld.m[0][2],(float)worldMat.matWorld.m[0][3],
			 (float)worldMat.matWorld.m[1][0],(float)worldMat.matWorld.m[1][1],(float)worldMat.matWorld.m[1][2],(float)worldMat.matWorld.m[1][3],
			 (float)worldMat.matWorld.m[2][0],(float)worldMat.matWorld.m[2][1],(float)worldMat.matWorld.m[2][2],(float)worldMat.matWorld.m[2][3],
			 (float)worldMat.matWorld.m[3][0],(float)worldMat.matWorld.m[3][1],(float)worldMat.matWorld.m[3][2],(float)worldMat.matWorld.m[3][3] };

	//view
	ViewMat view;
	view.matView = XMMatrixIdentity();


	//平行投影の射影行列生成
	ProjectionMat projection;

	projection.matProjection = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width,
		WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);

	cbt->constMapTransform->mat = matW * view.matView * projection.matProjection;
}

void Sprite::UpdateClipping(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop,
	const bool& isReverseX,const bool& isReverseY, float rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//反転
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	float texLeft = UVleftTop.x * +(float)length.x * scale;
	float texRight = (UVleftTop.x + UVlength.x) * +(float)length.x * scale;
	float texTop = UVleftTop.y * +(float)length.y * scale;
	float texBottom = (UVleftTop.y + UVlength.y) * +(float)length.y * scale;

	if (isPosLeftTop)
	{
		//左上からの座標
		vertices[0] = { {0,UVlength.y * length.y * scale,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices[2] = { {UVlength.x * length.x * scale,UVlength.y * length.y * scale,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices[3] = { {UVlength.x * length.x * scale,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	else
	{
		//切り抜いた後の画像の中心からの位置！！！！！！！！
		vertices[0] = { {-UVlength.x * length.x * scale / 2.0f,UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices[1] = { {-UVlength.x * length.x * scale / 2.0f,-UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices[2] = { {UVlength.x * length.x * scale / 2.0f,UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices[3] = { {UVlength.x * length.x * scale / 2.0f,-UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	constMapMaterial->color = color;

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);

	if (isPosLeftTop)
	{
		//引数の左上座標を設定
		worldMat.trans = { leftTop.x,leftTop.y,0 };
	}
	else
	{
		//切り抜いた後の画像の中心を設定！！！!!!!!!!!!!!!!!!!
		worldMat.trans = { leftTop.x + texLeft + UVlength.x * (float)length.x * scale / 2.0f,
			leftTop.y + texTop + UVlength.y * (float)length.y * scale / 2.0f,
			leftTop.z };
	}
	worldMat.SetWorld();


	//親がいたら
	if (worldMat.parent != nullptr)
	{
		worldMat.matWorld *= worldMat.parent->matWorld;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld.m[0][0],(float)worldMat.matWorld.m[0][1],(float)worldMat.matWorld.m[0][2],(float)worldMat.matWorld.m[0][3],
			 (float)worldMat.matWorld.m[1][0],(float)worldMat.matWorld.m[1][1],(float)worldMat.matWorld.m[1][2],(float)worldMat.matWorld.m[1][3],
			 (float)worldMat.matWorld.m[2][0],(float)worldMat.matWorld.m[2][1],(float)worldMat.matWorld.m[2][2],(float)worldMat.matWorld.m[2][3],
			 (float)worldMat.matWorld.m[3][0],(float)worldMat.matWorld.m[3][1],(float)worldMat.matWorld.m[3][2],(float)worldMat.matWorld.m[3][3] };

	//view
	ViewMat view;
	view.matView = XMMatrixIdentity();

	//平行投影の射影行列生成
	ProjectionMat projection;
	projection.matProjection = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width,
		WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);

	cbt->constMapTransform->mat = matW * view.matView * projection.matProjection;
}



