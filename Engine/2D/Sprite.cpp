#include "Sprite.h"
#include "CameraManager.h"
#include"BaseCollider.h"

using namespace DirectX;

RootPipe Sprite::spritePipelineSet_;

D3D12_INPUT_ELEMENT_DESC Sprite::sInputLayoutSprite_[2] = {
		{//xyz座標
		 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		 D3D12_APPEND_ALIGNED_ELEMENT,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		{//uv座標
		 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		 D3D12_APPEND_ALIGNED_ELEMENT,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
};

//--------------------------------------------------------------------------
Sprite::~Sprite()
{
}

Sprite::Sprite()
{
	//インスタンスの種類
	objInsType_ = ObjectInstanceType::SPRITE;

	//バッファ設定
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

//--------------------------------------------------------------------------
void Sprite::CommonInitialize()
{
	//パイプラインなどを設定
	PipeLineSetting(D3D12_FILL_MODE_SOLID, spritePipelineSet_,
		"Resources/shaders/SpriteVS.hlsl", "Resources/shaders/SpritePS.hlsl",
		sInputLayoutSprite_, _countof(sInputLayoutSprite_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, true);
}

void Sprite::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	//ワールド行列用クラスのインスタンスセット
	IObject::Initialize(std::move(worldMat));
}

//-------------------------------------------------------------------------------------
void Sprite::Update()
{
	IObject::Update();
}

//-------------------------------------------------------------------------------------------------------------------
void Sprite::Draw()
{
	//ポリモーフィズムで呼び出されるとき用に
	DrawBoxSprite(nullptr, { 0.5f,0.5f });
}

void Sprite::SpriteDraw()
{
	HRESULT result = {};

	//パイプラインなどセット
	IObject::SetRootPipe(&spritePipelineSet_, 0, spritePipelineSet_.rootSignature.Get());
	//メッシュの構造
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//色用のバッファをセット
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(COLOR, constBuffMaterial_->GetGPUVirtualAddress());

	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//テクスチャを設定していなかったら
	uint64_t texHL = texHandle_;
	TextureManager::CheckTexHandle(texHL);
	srvGpuHandle.ptr = texHL;
	//テクスチャ
	//SRVヒープの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetDescHeapPP());
	//srv指定
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(TEX, srvGpuHandle);

	//行列セット
	cbt_.DrawCommand(MATRIX);

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

void Sprite::DrawBoxSprite(Camera2D* camera,
	const Vec2& ancorUV, bool isReverseX, bool isReverseY)
{
	//更新
	Update();

	DrawUpdate(camera, { GetTrans().x, GetTrans().y }, { GetScale().x,GetScale().y },
		texHandle_, ancorUV, isReverseX, isReverseY, GetRot(), &cbt_);
}

void Sprite::DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	 bool isPosLeftTop, bool isReverseX, bool isReverseY)
{
	//更新
	Update();

	UpdateClipping(camera, { GetTrans().x,GetTrans().y }, { GetScale().x,GetScale().y },
		UVleftTop, UVlength, texHandle_,
		isPosLeftTop, isReverseX, isReverseY, { GetRot() }, &cbt_);
}

//-------------------------------------------------------------------------------------
void Sprite::DrawUpdate(Camera2D* camera, const Vec2& pos, const Vec2& scale,
	uint64_t textureHandle, const Vec2& ancorUV,
	bool isReverseX, bool isReverseY, const Vec3& rotation,
	ConstBuffTransform* cbt)
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
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	vertices_[0] = { {-(float)(length.x * scale.x * ancorUV.x),+(float)(length.y * scale.y * (1.0f - ancorUV.y)),0.0f},{0.0f,1.0f} };//左下
	vertices_[1] = { {-(float)(length.x * scale.x * ancorUV.x),-(float)(length.y * scale.y * (ancorUV.y)),0.0f},{0.0f,0.0f} };//左上
	vertices_[2] = { {+(float)(length.x * scale.x * (1.0f - ancorUV.x)),+(float)(length.y * scale.y * (1.0f - ancorUV.y)),0.0f},{1.0f,1.0f} };//右下
	vertices_[3] = { {+(float)(length.x * scale.x * (1.0f - ancorUV.x)),-(float)(length.y * scale.y * (ancorUV.y)),0.0f},{1.0f,0.0f} };//右上

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot_.x = rotation.x;
	worldMat.rot_.y = rotation.y;
	worldMat.rot_.z = rotation.z;
	worldMat.trans_ = { pos.x /*+ length.x * ancorUV.x * scale*/,pos.y/* + length.y * ancorUV.y * scale*/,0.0f };

	//行列計算、セット
	CalcAndSetMat(cbt, worldMat, camera);

	//描画
	SpriteDraw();
}

void Sprite::UpdateClipping(Camera2D* camera, const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	uint64_t textureHandle, bool isPosLeftTop,
	bool isReverseX, bool isReverseY, const Vec3& rotation, ConstBuffTransform* cbt)
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
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	float texLeft = UVleftTop.x * +(float)length.x * scale.x;
	float texTop = UVleftTop.y * +(float)length.y * scale.y;

	if (isPosLeftTop)
	{
		//左上からの座標
		vertices_[0] = { {0,UVlength.y * length.y * scale.y,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices_[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices_[2] = { {UVlength.x * length.x * scale.x,UVlength.y * length.y * scale.y,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices_[3] = { {UVlength.x * length.x * scale.x,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	else
	{
		//切り抜いた後の画像の中心からの位置！！！！！！！！
		vertices_[0] = { {-UVlength.x * length.x * scale.x / 2.0f,UVlength.y * length.y * scale.y / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		vertices_[1] = { {-UVlength.x * length.x * scale.x / 2.0f,-UVlength.y * length.y * scale.y / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		vertices_[2] = { {UVlength.x * length.x * scale.x / 2.0f,UVlength.y * length.y * scale.y / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		vertices_[3] = { {UVlength.x * length.x * scale.x / 2.0f,-UVlength.y * length.y * scale.y / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}

	//ワールド行列
	WorldMat worldMat;

	worldMat.rot_.x = rotation.x;
	worldMat.rot_.y = rotation.y;
	worldMat.rot_.z = rotation.z;

	if (isPosLeftTop)
	{
		//引数の左上座標を設定
		worldMat.trans_ = { leftTop.x,leftTop.y,0 };
	}
	else
	{
		//切り抜いた後の画像の中心を設定
		worldMat.trans_ = { leftTop.x + texLeft + UVlength.x * (float)length.x * scale.x / 2.0f,
			leftTop.y + texTop + UVlength.y * (float)length.y * scale.y / 2.0f,
			0 };
	}

	//行列計算、セット
	CalcAndSetMat(cbt, worldMat, camera);

	//描画
	SpriteDraw();
}

//--------------------------------------------------------
XMMATRIX Sprite::GetCameraMatrix(Camera2D* camera)
{
	ViewMat view;
	if (camera)
	{
		view.matView_ = camera->GetCameraMatrix();
	}
	else
	{
		view.matView_ = CameraManager::GetInstance().GetCamera2D()->GetCameraMatrix();
	}

	return view.matView_;
}

void Sprite::CalcAndSetMat(ConstBuffTransform* cbt, WorldMat& worldMat, Camera2D* camera)
{
	worldMat.CalcAllTreeMat();

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld_.m_[0][0],(float)worldMat.matWorld_.m_[0][1],(float)worldMat.matWorld_.m_[0][2],(float)worldMat.matWorld_.m_[0][3],
			 (float)worldMat.matWorld_.m_[1][0],(float)worldMat.matWorld_.m_[1][1],(float)worldMat.matWorld_.m_[1][2],(float)worldMat.matWorld_.m_[1][3],
			 (float)worldMat.matWorld_.m_[2][0],(float)worldMat.matWorld_.m_[2][1],(float)worldMat.matWorld_.m_[2][2],(float)worldMat.matWorld_.m_[2][3],
			 (float)worldMat.matWorld_.m_[3][0],(float)worldMat.matWorld_.m_[3][1],(float)worldMat.matWorld_.m_[3][2],(float)worldMat.matWorld_.m_[3][3] };

	//view
	ViewMat view;
	view.matView_ = GetCameraMatrix(camera);

	//平行投影の射影行列生成
	ProjectionMat projection;
	projection.matProjection_ = XMMatrixOrthographicOffCenterLH(0.0f, WindowsApp::GetInstance().WINDOW_WIDTH_,
		WindowsApp::GetInstance().WINDOW_HEIGHT_, 0.0, 0.0f, 1.0f);

	cbt->SetWorldMat(matW);
	cbt->SetViewProjMat(view.matView_ * projection.matProjection_);
	cbt->SetCameraPos(view.eye_);
}



