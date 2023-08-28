#include "Sprite.h"

using namespace DirectX;


void Sprite::Initialize()
{
	//sprite用
	{
		uint32_t sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB = static_cast<uint32_t>(sizeof(vertices_[0]) * 4.0);
		//頂点バッファの設定		//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//頂点バッファの生成
		BuffProperties(heapProp, resDesc, vertBuff_.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbView_.BufferLocation = vertBuff_.Get()->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbView_.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbView_.StrideInBytes = sizeof(vertices_[0]);
	}
}

void SpriteCommonBeginDraw(RootPipe* pipelineSet)
{
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::SpriteDraw()
{
	HRESULT result = {};

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	VertexSprite* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int32_t i = 0; i < 4; i++) {
		vertMap[i] = vertices_[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff_->Unmap(0, nullptr);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Update(const Vec2& pos, const Vec2& scale,
	const Vec4& color, uint64_t textureHandle, const Vec2& ancorUV,
	bool isReverseX, bool isReverseY, float rotation,
	ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	//テクスチャを設定していなかったら
	uint64_t textureHandle_;

	if (textureHandle == NULL)
	{
		textureHandle_ = TextureManager::GetWhiteTexHandle();
	}
	else
	{
		textureHandle_ = textureHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = TextureManager::GetTexBuff()[(textureHandle_ - srvGpuHandle.ptr) / DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetHeapDesc().Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//反転
	if (isReverseX)length.x_ *= -1;
	if (isReverseY)length.y_ *= -1;

	vertices_[0] = { {-(float)(length.x_ * scale.x_ * ancorUV.x_),+(float)(length.y_ * scale.y_ * (1.0f - ancorUV.y_)),0.0f},{0.0f,1.0f} };//左下
	vertices_[1] = { {-(float)(length.x_ * scale.x_ * ancorUV.x_),-(float)(length.y_ * scale.y_ * (ancorUV.y_)),0.0f},{0.0f,0.0f} };//左上
	vertices_[2] = { {+(float)(length.x_ * scale.x_ * (1.0f - ancorUV.x_)),+(float)(length.y_ * scale.y_ * (1.0f - ancorUV.y_)),0.0f},{1.0f,1.0f} };//右下
	vertices_[3] = { {+(float)(length.x_ * scale.x_ * (1.0f - ancorUV.x_)),-(float)(length.y_ * scale.y_ * (ancorUV.y_)),0.0f},{1.0f,0.0f} };//右上


	constMapMaterial->color = color;

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot_.z_ = AngletoRadi(rotation);
	worldMat.trans_ = { pos.x_ /*+ length.x * ancorUV.x * scale*/,pos.y_/* + length.y * ancorUV.y * scale*/,0.0f };
	worldMat.CalcWorldMat();

	//親がいたら
	if (worldMat.parent_ != nullptr)
	{
		worldMat.matWorld_ *= worldMat.parent_->matWorld_;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld_.m_[0][0],(float)worldMat.matWorld_.m_[0][1],(float)worldMat.matWorld_.m_[0][2],(float)worldMat.matWorld_.m_[0][3],
			 (float)worldMat.matWorld_.m_[1][0],(float)worldMat.matWorld_.m_[1][1],(float)worldMat.matWorld_.m_[1][2],(float)worldMat.matWorld_.m_[1][3],
			 (float)worldMat.matWorld_.m_[2][0],(float)worldMat.matWorld_.m_[2][1],(float)worldMat.matWorld_.m_[2][2],(float)worldMat.matWorld_.m_[2][3],
			 (float)worldMat.matWorld_.m_[3][0],(float)worldMat.matWorld_.m_[3][1],(float)worldMat.matWorld_.m_[3][2],(float)worldMat.matWorld_.m_[3][3] };

	//view
	ViewMat view;
	view.matView_ = XMMatrixIdentity();


	//平行投影の射影行列生成
	ProjectionMat projection;

	projection.matProjection_ = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().WINDOW_WIDTH_,
		WindowsApp::GetInstance().WINDOW_HEIGHT_, 0.0, 0.0f, 1.0f);

	cbt->SetWorldMat(matW);
	cbt->SetViewProjMat(view.matView_ * projection.matProjection_);
	cbt->SetCameraPos(view.eye_);
}

void Sprite::UpdateClipping(const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	const Vec4& color, uint64_t textureHandle, bool isPosLeftTop,
	bool isReverseX, bool isReverseY, float rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	//テクスチャを設定していなかったら
	uint64_t textureHandle_;

	if (textureHandle == NULL)
	{
		textureHandle_ = TextureManager::GetWhiteTexHandle();
	}
	else
	{
		textureHandle_ = textureHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = TextureManager::GetInstance().GetTexBuff()[(textureHandle_ - srvGpuHandle.ptr) / DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetHeapDesc().Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//反転
	if (isReverseX)length.x_ *= -1;
	if (isReverseY)length.y_ *= -1;

	float texLeft = UVleftTop.x * +(float)length.x_ * scale.x_;
	float texRight = (UVleftTop.x + UVlength.x) * +(float)length.x_ * scale.x_;
	float texTop = UVleftTop.y * +(float)length.y_ * scale.y_;
	float texBottom = (UVleftTop.y + UVlength.y) * +(float)length.y_ * scale.y_;

	if (isPosLeftTop)
	{
		//左上からの座標
		vertices_[0] = { {0,UVlength.y * length.y_ * scale.y_,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices_[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices_[2] = { {UVlength.x * length.x_ * scale.x_,UVlength.y * length.y_ * scale.y_,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices_[3] = { {UVlength.x * length.x_ * scale.x_,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	else
	{
		//切り抜いた後の画像の中心からの位置！！！！！！！！
		vertices_[0] = { {-UVlength.x * length.x_ * scale.x_ / 2.0f,UVlength.y * length.y_ * scale.y_ / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices_[1] = { {-UVlength.x * length.x_ * scale.x_ / 2.0f,-UVlength.y * length.y_ * scale.y_ / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices_[2] = { {UVlength.x * length.x_ * scale.x_ / 2.0f,UVlength.y * length.y_ * scale.y_ / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices_[3] = { {UVlength.x * length.x_ * scale.x_ / 2.0f,-UVlength.y * length.y_ * scale.y_ / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	constMapMaterial->color = color;

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot_.z_ = AngletoRadi(rotation);

	if (isPosLeftTop)
	{
		//引数の左上座標を設定
		worldMat.trans_ = { leftTop.x_,leftTop.y_,0 };
	}
	else
	{
		//切り抜いた後の画像の中心を設定
		worldMat.trans_ = { leftTop.x_ + texLeft + UVlength.x * (float)length.x_ * scale.x_ / 2.0f,
			leftTop.y_ + texTop + UVlength.y * (float)length.y_ * scale.y_ / 2.0f,
			0 };
	}
	worldMat.CalcWorldMat();


	//親がいたら
	if (worldMat.parent_ != nullptr)
	{
		worldMat.matWorld_ *= worldMat.parent_->matWorld_;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld_.m_[0][0],(float)worldMat.matWorld_.m_[0][1],(float)worldMat.matWorld_.m_[0][2],(float)worldMat.matWorld_.m_[0][3],
			 (float)worldMat.matWorld_.m_[1][0],(float)worldMat.matWorld_.m_[1][1],(float)worldMat.matWorld_.m_[1][2],(float)worldMat.matWorld_.m_[1][3],
			 (float)worldMat.matWorld_.m_[2][0],(float)worldMat.matWorld_.m_[2][1],(float)worldMat.matWorld_.m_[2][2],(float)worldMat.matWorld_.m_[2][3],
			 (float)worldMat.matWorld_.m_[3][0],(float)worldMat.matWorld_.m_[3][1],(float)worldMat.matWorld_.m_[3][2],(float)worldMat.matWorld_.m_[3][3] };

	//view
	ViewMat view;
	view.matView_ = XMMatrixIdentity();

	//平行投影の射影行列生成
	ProjectionMat projection;
	projection.matProjection_ = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().WINDOW_WIDTH_,
		WindowsApp::GetInstance().WINDOW_HEIGHT_, 0.0, 0.0f, 1.0f);

	cbt->SetWorldMat(matW);
	cbt->SetViewProjMat(view.matView_ * projection.matProjection_);
	cbt->SetCameraPos(view.eye_);
}



