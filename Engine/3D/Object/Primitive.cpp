#include "Primitive.h"
#include "Util.h"

using namespace DirectX;
using namespace Constant;


uint16_t Primitive::sIndicesBox_[S_BOX_INDICES_NUM_] =
{
	0,1,2,//三角形1つ目
	2,1,3,//三角形2つ目
};
uint16_t Primitive::sIndicesTriangle_[S_TRIANGLE_INDICES_NUM_] =
{
	0,1,2//三角形2つ目
};
uint16_t Primitive::sIndicesCube_[S_CUBE_INDICES_NUM_] =
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

uint16_t Primitive::sIndicesCircle_[S_CIRCLE_INDICES_NUM_] =
{
	2,1,0,
	3,2,0,
	4,3,0,
	5,4,0,
	6,5,0,
	7,6,0,
	8,7,0,
	9,8,0,
	10,9,0,
	11,10,0,
	12,11,0,
	13,12,0,
	14,13,0,
	15,14,0,
	16,15,0,
	17,16,0,
	18,17,0,
	19,18,0,
	20,19,0,
	21,20,0,
	22,21,0,
	23,22,0,
};

uint16_t Primitive::sIndicesLine_[S_LINE_INDICES_NUM_] =
{
	0,1
};

uint16_t Primitive::sIndicesSphere_[S_SPHERE_INDICES_NUM_ * S_SPHERE_WIDTH_];

uint32_t Primitive::sizeVB_;

//------------------------------------------------------------------------------------------------------

void Primitive::Initialize()
{
	InitializeTriangle();
	InitializeBox();
	InitializeCircle();
	InitializeLine();
	InitializeCube();
	InitializeSphere();
}

void Primitive::InitializeTriangle()
{
	HRESULT result = {};

	float angle = TRIANGLE_ANGLE_1_;
	float angle2 = TRIANGLE_ANGLE_2_;
	float angle3 = TRIANGLE_ANGLE_3_;

	verticesTriangle_[0] = { {sinf(angle),cosf(angle),0},{verticesTriangle_[0].normal},  {0.0f,UV_MAX} ,{} };//左下
	verticesTriangle_[1] = { {0,cosf(angle2),0},{verticesTriangle_[1].normal},           {UV_HALF,0.0f} ,{} };//上
	verticesTriangle_[2] = { {sinf(angle3),cosf(angle3),0},{verticesTriangle_[2].normal},{UV_MAX,UV_MAX} ,{} };//右下


	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesTriangle_[0]) * _countof(verticesTriangle_));

	//頂点バッファの設定		//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//頂点バッファの生成
	BuffProperties(heapProp_, resDesc, &vertBuffTriangle_);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbTriangleView_.BufferLocation = vertBuffTriangle_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbTriangleView_.SizeInBytes = sizeVB_;
	// 頂点1つ分のデータサイズ
	vbTriangleView_.StrideInBytes = sizeof(verticesTriangle_[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesTriangle_));

		//リソース設定
		resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesTriangle_); i++)
		{
			indexMap[i] = sIndicesTriangle_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewTriangle_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewTriangle_.Format = DXGI_FORMAT_R16_UINT;
		ibViewTriangle_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesTriangle_) / S_MESH_VERTEX_NUM_NORMAL_; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			uint16_t index0 = sIndicesTriangle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 0];
			uint16_t index1 = sIndicesTriangle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 1];
			uint16_t index2 = sIndicesTriangle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesTriangle_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesTriangle_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesTriangle_[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesTriangle_[index0].normal, normal);
			XMStoreFloat3(&verticesTriangle_[index1].normal, normal);
			XMStoreFloat3(&verticesTriangle_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeBox()
{
	HRESULT result = {};

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesBox_[0]) * _countof(verticesBox_));

	//頂点バッファの設定		//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//頂点バッファの生成
	BuffProperties(heapProp_, resDesc, &vertBuffBox_);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbBoxView_.BufferLocation = vertBuffBox_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbBoxView_.SizeInBytes = sizeVB_;
	// 頂点1つ分のデータサイズ
	vbBoxView_.StrideInBytes = sizeof(verticesBox_[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesBox_));

		//リソース設定
		resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesBox_); i++)
		{
			indexMap[i] = sIndicesBox_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewBox_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewBox_.Format = DXGI_FORMAT_R16_UINT;
		ibViewBox_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesBox_) / IModel::S_MESH_VERTEX_NORMAL_NUM_; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			uint16_t index0 = sIndicesBox_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 0];
			uint16_t index1 = sIndicesBox_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 1];
			uint16_t index2 = sIndicesBox_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesBox_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesBox_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesBox_[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesBox_[index0].normal, normal);
			XMStoreFloat3(&verticesBox_[index1].normal, normal);
			XMStoreFloat3(&verticesBox_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCircle()
{
	HRESULT result = {};

	verticesCircle_[0].pos = { 0.0f,0.0f,0.0f };

	static float sCount = _countof(verticesCircle_) - S_CENTER_FIRST_VERTEX_NUM_;//中心点と初期の点はカウントしない

	for (int32_t i = 1; i < _countof(verticesCircle_); i++)
	{
		verticesCircle_[i].pos = { cosf(AngletoRadi(PI_2 / sCount) * (i - 1)), sinf(AngletoRadi(PI_2 / sCount) * (i - 1)),0 };
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesCircle_[0]) * _countof(verticesCircle_));

	//頂点バッファの設定		//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//頂点バッファの生成
	BuffProperties(heapProp_, resDesc, &vertBuffCircle_);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbCircleView_.BufferLocation = vertBuffCircle_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbCircleView_.SizeInBytes = sizeVB_;
	// 頂点1つ分のデータサイズ
	vbCircleView_.StrideInBytes = sizeof(verticesCircle_[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesCircle_));

		//リソース設定
		resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesCircle_); i++)
		{
			indexMap[i] = sIndicesCircle_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewCircle_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCircle_.Format = DXGI_FORMAT_R16_UINT;
		ibViewCircle_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesCircle_) / IModel::S_MESH_VERTEX_NORMAL_NUM_; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			uint16_t index0 = sIndicesCircle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 0];
			uint16_t index1 = sIndicesCircle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 1];
			uint16_t index2 = sIndicesCircle_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesCircle_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCircle_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCircle_[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesCircle_[index0].normal, normal);
			XMStoreFloat3(&verticesCircle_[index1].normal, normal);
			XMStoreFloat3(&verticesCircle_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCube()
{
	HRESULT result = {};

	{
		//手前
		verticesCube_[0] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_,-NORMAL_RADIUS_},{verticesCube_[0].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[1] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[1].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[2] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[2].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[3] = { {NORMAL_RADIUS_,NORMAL_RADIUS_,  -NORMAL_RADIUS_},{verticesCube_[3].normal},{UV_MAX,0.0f} ,{} };//右上

		verticesCube_[4] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_,NORMAL_RADIUS_},{verticesCube_[4].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[5] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[5].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[6] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[6].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[7] = { {NORMAL_RADIUS_,NORMAL_RADIUS_,  NORMAL_RADIUS_},{verticesCube_[7].normal},{UV_MAX,0.0f} ,{} };//右上
		//上
		verticesCube_[8] = { {NORMAL_RADIUS_,NORMAL_RADIUS_,-NORMAL_RADIUS_},{verticesCube_[8].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[9] = { {NORMAL_RADIUS_,NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[9].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[10] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[10].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[11] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[11].normal},{UV_MAX,0.0f} ,{} };//右上

		verticesCube_[12] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_,-NORMAL_RADIUS_},{verticesCube_[12].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[13] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[13].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[14] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[14].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[15] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[15].normal},{UV_MAX,0.0f} ,{} };//右上

		verticesCube_[16] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_,-NORMAL_RADIUS_},{verticesCube_[16].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[17] = { {-NORMAL_RADIUS_,-NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[17].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[18] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[18].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[19] = { {-NORMAL_RADIUS_,NORMAL_RADIUS_,  NORMAL_RADIUS_},{verticesCube_[19].normal},{UV_MAX,0.0f} ,{} };//右上

		verticesCube_[20] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_,-NORMAL_RADIUS_},{verticesCube_[20].normal},{0.0f,UV_MAX} ,{} };//左下
		verticesCube_[21] = { {NORMAL_RADIUS_,-NORMAL_RADIUS_, NORMAL_RADIUS_},{verticesCube_[21].normal},{0.0f,0.0f} ,{} };//左上
		verticesCube_[22] = { {NORMAL_RADIUS_,NORMAL_RADIUS_, -NORMAL_RADIUS_},{verticesCube_[22].normal},{UV_MAX,UV_MAX} ,{} };//右下
		verticesCube_[23] = { {NORMAL_RADIUS_,NORMAL_RADIUS_,  NORMAL_RADIUS_},{verticesCube_[23].normal},{UV_MAX,0.0f} ,{} };//右上;//左下
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesCube_[0]) * _countof(verticesCube_));

	//頂点バッファの設定		//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//頂点バッファの生成
	BuffProperties(heapProp_, resDesc, &vertBuffCube_);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbCubeView_.BufferLocation = vertBuffCube_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbCubeView_.SizeInBytes = sizeVB_;
	// 頂点1つ分のデータサイズ
	vbCubeView_.StrideInBytes = sizeof(verticesCube_[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesCube_));

		//リソース設定
		resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesCube_); i++)
		{
			indexMap[i] = sIndicesCube_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewCube_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCube_.Format = DXGI_FORMAT_R16_UINT;
		ibViewCube_.SizeInBytes = sizeIB;
	}


	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesCube_) / IModel::S_MESH_VERTEX_NORMAL_NUM_; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			uint16_t index0 = sIndicesCube_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 0];
			uint16_t index1 = sIndicesCube_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 1];
			uint16_t index2 = sIndicesCube_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesCube_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCube_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCube_[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesCube_[index0].normal, normal);
			XMStoreFloat3(&verticesCube_[index1].normal, normal);
			XMStoreFloat3(&verticesCube_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeLine()
{
	HRESULT result = {};

	verticesLine_[0] = { {-NORMAL_RADIUS_,0,0},{verticesLine_[0].normal},{0.0f,UV_MAX} ,{} };//左下
	verticesLine_[1] = { {NORMAL_RADIUS_,0,0},  {verticesLine_[1].normal},{UV_HALF,0.0f} ,{} };//上

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesLine_));

		//リソース設定
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesLine_); i++)
		{
			indexMap[i] = sIndicesLine_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewLine_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewLine_.Format = DXGI_FORMAT_R16_UINT;
		ibViewLine_.SizeInBytes = sizeIB;
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesLine_[0]) * _countof(verticesLine_));

	//頂点バッファの設定		//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//頂点バッファの生成
	BuffProperties(heapProp_, resDesc, &vertBuffLine_);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbLineView_.BufferLocation = vertBuffLine_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbLineView_.SizeInBytes = sizeVB_;
	// 頂点1つ分のデータサイズ
	vbLineView_.StrideInBytes = sizeof(verticesLine_[0]);
}

void Primitive::InitializeSphere()
{
	HRESULT result = {};

	//球体用
	{
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB_ = static_cast<uint32_t>(sizeof(verticesSphere_[0]) * (_countof(verticesSphere_)));

		//頂点バッファの設定		//ヒープ設定
		heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeVB_);

		//頂点バッファの生成
		BuffProperties(heapProp_, resDesc, vertBuffSphere_.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbViewSphere_.BufferLocation = vertBuffSphere_->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbViewSphere_.SizeInBytes = sizeVB_;
		// 頂点1つ分のデータサイズ
		vbViewSphere_.StrideInBytes = sizeof(verticesSphere_[0]);


		WorldMat worldMat;
		Vec3 vec = { 0,-NORMAL_RADIUS_,0 };

		//頂点二つ
		verticesSphere_[0] = { {vec.x,vec.y,vec.z},{},{UV_MAX,0.0f} ,{} };//下
		verticesSphere_[1] = { {vec.x,-vec.y,vec.z},{},{UV_MAX,0.0f} ,{} };//上

		for (int32_t i = 0; i < S_SPHERE_WIDTH_; i++)//横
		{
			//頂点間の角度
			worldMat.rot_.y = (float)i * PI_2 / (float)(S_SAME_COLUMN_PAIR_NUM_);


			for (int32_t j = 0; j < S_SPHERE_HEIGHT_; j++)//縦
			{
				worldMat.rot_.x = ((float)(j + 1) * PI_2 / (float)(S_SAME_COLUMN_PAIR_NUM_));
				worldMat.CalcWorldMat();
				vec = { 0,-NORMAL_RADIUS_,0 };
				Vec3xM4(vec, worldMat.matWorld_, false);

				int32_t p = i * S_SPHERE_HEIGHT_ + j;
				verticesSphere_[PAIR_ + p] = { {vec.x,vec.y,vec.z},{},{UV_MAX,0.0f} ,{} };
			}
		}

		uint16_t count = 0;
		uint16_t count2 = 0;
		uint16_t count3 = 0;
		uint16_t count4 = 0;
		bool isLast = false;
		//インデックス
		for (uint32_t i = 0; i < _countof(sIndicesSphere_); i++)
		{
			if (i % (S_SPHERE_INDICES_NUM_ * (S_SAME_COLUMN_PAIR_NUM_)) == 0 && i != 0)//最後の縦の列
			{
				isLast = true;
			}

			//一番下か上じゃなければ
			if (!(i % S_SPHERE_INDICES_NUM_ == 0 || i % (S_SPHERE_INDICES_NUM_ * (count + 1) - IModel::S_MESH_VERTEX_NORMAL_NUM_) == 0))
			{
				if (count2 % PAIR_ == 0)
				{
					if (isLast)
					{
						sIndicesSphere_[i] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count3);
						sIndicesSphere_[i + 1] = PAIR_ + 0 + (count3);//一周してきたので一番最初の列を使う
						sIndicesSphere_[i + 2] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count3 + 1);

						count3++;
						i += PAIR_;
					}
					else
					{
						sIndicesSphere_[i] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count3);
						sIndicesSphere_[i + 1] = PAIR_ + S_SPHERE_HEIGHT_ * (count + 1) + (count3);
						sIndicesSphere_[i + 2] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count3 + 1);

						count3++;
						i += PAIR_;
					}
				}
				else if (count2 % PAIR_ == 1)
				{
					if (isLast)
					{
						sIndicesSphere_[i] = PAIR_ + 0 + (count4 + 1);//一周してきたので一番最初の列を使う
						sIndicesSphere_[i + 1] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count4 + 1);
						sIndicesSphere_[i + 2] = PAIR_ + 0 + (count4);//一周してきたので一番最初の列を使う

						count4++;
						i += PAIR_;
					}
					else
					{
						sIndicesSphere_[i] = PAIR_ + S_SPHERE_HEIGHT_ * (count + 1) + (count4 + 1);
						sIndicesSphere_[i + 1] = PAIR_ + S_SPHERE_HEIGHT_ * count + (count4 + 1);
						sIndicesSphere_[i + 2] = PAIR_ + S_SPHERE_HEIGHT_ * (count + 1) + (count4);

						count4++;
						i += PAIR_;
					}
				}

				count2++;
			}
			if (i % S_SPHERE_INDICES_NUM_ == 0 || i % (S_SPHERE_INDICES_NUM_ * (count + 1) - IModel::S_MESH_VERTEX_NORMAL_NUM_) == 0)
			{
				if (i % S_SPHERE_INDICES_NUM_ == 0)//一番下の三角形
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 0;
						sIndicesSphere_[i + 1] = PAIR_ + 0;
						sIndicesSphere_[i + 2] = PAIR_ + S_SPHERE_HEIGHT_ * count;

						i += PAIR_;
					}
					else
					{
						sIndicesSphere_[i] = PAIR_ + S_SPHERE_HEIGHT_ * (count + 1);
						sIndicesSphere_[i + 1] = PAIR_ + S_SPHERE_HEIGHT_ * count;
						sIndicesSphere_[i + 2] = 0;

						i += PAIR_;
					}
				}
				else if (i % (S_SPHERE_INDICES_NUM_ * (count + 1) - IModel::S_MESH_VERTEX_NORMAL_NUM_) == 0)//一番上の三角形
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 1 + S_SPHERE_HEIGHT_ * (count + 1);
						sIndicesSphere_[i + 1] = S_SAME_COLUMN_PAIR_NUM_;
						sIndicesSphere_[i + 2] = 1;

						i += PAIR_;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
					else
					{
						sIndicesSphere_[i] = 1 + S_SPHERE_HEIGHT_ * (count + 1);
						sIndicesSphere_[i + 1] = 1 + S_SPHERE_HEIGHT_ * (count + 2);
						sIndicesSphere_[i + 2] = 1;

						i += PAIR_;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
				}
			}
		}

		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesSphere_));

		//リソース設定
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int32_t i = 0; i < _countof(sIndicesSphere_); i++)
		{
			indexMap[i] = sIndicesSphere_[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewSphere_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewSphere_.Format = DXGI_FORMAT_R16_UINT;
		ibViewSphere_.SizeInBytes = sizeIB;


		//06_03
		/*if (indexNum == SPHERE)*/
		{
			for (int32_t i = 0; i < _countof(sIndicesSphere_) / IModel::S_MESH_VERTEX_NORMAL_NUM_; i++)
			{//三角形一つごとに計算
				//三角形のインデックスを取り出して、一時的な変数に入れる
				uint16_t index0 = sIndicesSphere_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 0];
				uint16_t index1 = sIndicesSphere_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 1];
				uint16_t index2 = sIndicesSphere_[i * IModel::S_MESH_VERTEX_NORMAL_NUM_ + 2];
				//三角形を構成する頂点座標をベクトルに代入
				XMVECTOR p0 = XMLoadFloat3(&verticesSphere_[index0].pos);
				XMVECTOR p1 = XMLoadFloat3(&verticesSphere_[index1].pos);
				XMVECTOR p2 = XMLoadFloat3(&verticesSphere_[index2].pos);
				//p0->p1ベクトル、p0->p2ベクトルを計算
				XMVECTOR v1 = XMVectorSubtract(p1, p0);
				XMVECTOR v2 = XMVectorSubtract(p2, p0);
				//外積（垂直なベクトル）
				XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
				//求めた法線を頂点データに代入
				XMStoreFloat3(&verticesSphere_[index0].normal, normal);
				XMStoreFloat3(&verticesSphere_[index1].normal, normal);
				XMStoreFloat3(&verticesSphere_[index2].normal, normal);
			}
		}
	}
}

void Primitive::DrawCommandPrimitive(ID3D12Resource* vertBuff, int32_t vertexCount, Vertex* vertex, const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
	const D3D12_VERTEX_BUFFER_VIEW& buffView, const D3D12_INDEX_BUFFER_VIEW& ibView, int32_t indicesCount,
	const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	HRESULT result = {};

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int32_t i = 0; i < vertexCount; i++) {

		vertMap[i] = vertex[i]; // 座標をコピー

	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// パイプラインステートとルートシグネチャの設定コマンド
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(primitiveTopology);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &buffView);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView);

	//マテリアルとかテクスチャ
	if (setMaterialLightTex)
	{
		setMaterialLightTex();
	}

	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indicesCount, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void Primitive::TriangleDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffTriangle_.Get(), _countof(verticesTriangle_), verticesTriangle_, topology,
		vbTriangleView_, ibViewTriangle_,
		_countof(sIndicesTriangle_),
		setRootParam, setMaterialLightTex);
}

void Primitive::BoxDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffBox_.Get(), _countof(verticesBox_), verticesBox_, topology,
		vbBoxView_, ibViewBox_,
		_countof(sIndicesBox_),
		setRootParam, setMaterialLightTex);
}

void Primitive::CircleDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffCircle_.Get(), _countof(verticesCircle_), verticesCircle_, topology,
		vbCircleView_, ibViewCircle_,
		_countof(sIndicesCircle_),
		setRootParam, setMaterialLightTex);
}

void Primitive::CubeDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffCube_.Get(), _countof(verticesCube_), verticesCube_, topology,
		vbCubeView_, ibViewCube_,
		_countof(sIndicesCube_),
		setRootParam, setMaterialLightTex);
}

void Primitive::LineDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffLine_.Get(), _countof(verticesLine_), verticesLine_, D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		vbLineView_, ibViewLine_,
		_countof(sIndicesLine_),
		setRootParam, setMaterialLightTex);
}

void Primitive::SphereDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffSphere_.Get(), _countof(verticesSphere_), verticesSphere_, topology,
		vbViewSphere_, ibViewSphere_,
		_countof(sIndicesSphere_),
		setRootParam, setMaterialLightTex);
}


