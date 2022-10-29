#include "Draw.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>


//設定をもとにSRV用デスクリプタヒープを生成
ComPtr < ID3D12DescriptorHeap> srvHeap = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
int count = 0;

ComPtr<ID3D12Resource> texBuff[512];

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
D3D12_INDEX_BUFFER_VIEW ibView3{};

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

static unsigned short indicesLine[2] =
{
	0,1//三角形2つ目
};


//いろんな図形用
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

//スプライト用
VertexSprite verticesS[4];

//球体
Vertex verticesSphere[2 + 34 * 36];
// 頂点バッファビューの作成
D3D12_VERTEX_BUFFER_VIEW vbView2{};
const int PAPA = 66 * 3 + 6;
//頂点バッファの生成
ComPtr<ID3D12Resource> vertBuff2 = nullptr;
static unsigned short indicesSphere[PAPA * 36];

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

//sprite
D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	} // (1行で書いたほうが見やすい)
};
SpriteSet pipelineSet;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER rootParams[3] = {};
UINT sizeVB;

// パイプランステートの生成
ComPtr < ID3D12PipelineState> pipelineState[3] = { nullptr };
// ルートシグネチャ
ComPtr<ID3D12RootSignature> rootSignature;
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
	

	//球体用
	{
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB = static_cast<UINT>(sizeof(verticesSphere[0]) * (_countof(verticesSphere)));

		//頂点バッファの設定		//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		ResourceProperties(resDesc, sizeVB);

		//頂点バッファの生成
		BuffProperties(heapProp, resDesc, vertBuff2.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbView2.BufferLocation = vertBuff2->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbView2.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbView2.StrideInBytes = sizeof(verticesSphere[0]);


		WorldMat worldMat;
		Vec3 vec={ 0,-5.0f,0 };

		//頂点二つ
		verticesSphere[0] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };//下
		verticesSphere[1] = { {vec.x,-vec.y,vec.z},{},{1.0f,0.0f} };//上

		for (int i = 0; i < 36; i++)//横
		{
			worldMat.rot.y = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int j = 0; j < 34; j++)//縦
			{
				worldMat.rot.x = ((float)(j+1) * (pi / 35.0f));
				worldMat.SetWorld();
				vec = { 0,-5.0f,0 };
				Vec3xM4(vec, worldMat.matWorld, false);

				int p = i * 34 + j;
				verticesSphere[(2) + i * 34 + j ] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };
			}
		}

		int count = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		bool isLast = false;
		//インデックス
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			if (i % (PAPA * 35) == 0 && i != 0)//最後の縦の列
			{
				isLast = true;
			}

			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{

			}
			else if (1)//一番下か上じゃなければ
			{
				if (count2 % 2 == 0)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 0 + (count3);//一周してきたので一番最初の列を使う
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 34 * (count + 1) + (count3);
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
				}
				else if (count2 % 2 == 1)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 0 + (count4 + 1);//一周してきたので一番最初の列を使う
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 0 + (count4);//一周してきたので一番最初の列を使う

						count4++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1) + (count4 + 1);
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 34 * (count + 1) + (count4);

						count4++;
						i += 2;
					}
				}

				count2++;
			}
			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{
				if (i % PAPA == 0)//一番下の三角形
				{
					if (isLast)
					{
						indicesSphere[i] = 0;
						indicesSphere[i + 1] = 2 + 0;
						indicesSphere[i + 2] = 2 + 34 * count;

						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1);
						indicesSphere[i + 1] = 2 + 34 * count;
						indicesSphere[i + 2] = 0;

						i += 2;
					}
				}
				else if (i % (PAPA * (count + 1) - 3) == 0)//一番上の三角形
				{
					if (isLast)
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 35;
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
					else
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 1 + 34 * (count + 2);
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
				}
			}
			/*if (i % 198 == 0)
			{
				count++;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}

			if (count2 % 2 == 0)
			{
				indicesSphere[i] = 35 * count - (count3 + 1);
				indicesSphere[i + 1] = 35 * (count + 1) - count3;
				indicesSphere[i + 2] = 35 * count - (count3);

				count3++;
				i += 2;
			}
			else if (count2 % 2 == 1)
			{
				indicesSphere[i] = 35 * (count + 1) - (count4);
				indicesSphere[i + 1] = 35 * count - (count4 + 1);
				indicesSphere[i + 2] =  35 * (count + 1) - (count4 + 1);

				count4++;
				i += 2;*/
				//}

		}

		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesSphere));

		//リソース設定
		ResourceProperties(resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			indexMap[i] = indicesSphere[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibView3.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView3.Format = DXGI_FORMAT_R16_UINT;
		ibView3.SizeInBytes = sizeIB;
	}


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
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineState->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 1)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 2)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob, LINE);

	//sprite用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSet.pipelineState.GetAddressOf(),
		pipelineSet.rootSignature.GetAddressOf(), pipelineSet.vsBlob, 
		pipelineSet.psBlob, SPRITE);

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

	//06_03
	/*if (indexNum == SPHERE)*/
	{
		for (int i = 0; i < _countof(indicesSphere) / 3; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indicesSphere[i * 3 + 0];
			unsigned short index1 = indicesSphere[i * 3 + 1];
			unsigned short index2 = indicesSphere[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesSphere[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesSphere[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesSphere[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesSphere[index0].normal, normal);
			XMStoreFloat3(&verticesSphere[index1].normal, normal);
			XMStoreFloat3(&verticesSphere[index2].normal, normal);
		}
	}
}

Draw::Draw()
{
	//sprite用
	{
		UINT sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB = static_cast<UINT>(sizeof(verticesS[0]) * 4.0);
		//頂点バッファの設定		//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//頂点バッファの生成
		BuffProperties(heapProp, resDesc, vertBuffS.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbViewS.BufferLocation = vertBuffS.Get()->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbViewS.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbViewS.StrideInBytes = sizeof(verticesS[0]);//kokogamatigatteita(vertices[0]ni!)
	}

	//行列
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
	
	{
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
		/*constMapMaterial->color = { 0,0,0,1 };*/

		//06_03
		//こいつらのせいでチカチカする！（Draw のインスタンスが生成されるたびに、法線が上書きされるせい！）
		///*else if (indexNum == TRIANGLE)*/
		//{
		//	for (int i = 0; i < _countof(indices2) / 3; i++)
		//	{//三角形一つごとに計算
		//		//三角形のインデックスを取り出して、一時的な変数に入れる
		//		unsigned short index0 = indices2[i * 3 + 0];
		//		unsigned short index1 = indices2[i * 3 + 1];
		//		unsigned short index2 = indices2[i * 3 + 2];
		//		//三角形を構成する頂点座標をベクトルに代入
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1ベクトル、p0->p2ベクトルを計算
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//外積（垂直なベクトル）
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//求めた法線を頂点データに代入
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == BOX)*/
		//{
		//	for (int i = 0; i < _countof(indices) / 3; i++)
		//	{//三角形一つごとに計算
		//		//三角形のインデックスを取り出して、一時的な変数に入れる
		//		unsigned short index0 = indices[i * 3 + 0];
		//		unsigned short index1 = indices[i * 3 + 1];
		//		unsigned short index2 = indices[i * 3 + 2];
		//		//三角形を構成する頂点座標をベクトルに代入
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1ベクトル、p0->p2ベクトルを計算
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//外積（垂直なベクトル）
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//求めた法線を頂点データに代入
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == CUBE)*/
		//{
		//	for (int i = 0; i < _countof(indicesCube) / 3; i++)
		//	{//三角形一つごとに計算
		//		//三角形のインデックスを取り出して、一時的な変数に入れる
		//		unsigned short index0 = indicesCube[i * 3 + 0];
		//		unsigned short index1 = indicesCube[i * 3 + 1];
		//		unsigned short index2 = indicesCube[i * 3 + 2];
		//		//三角形を構成する頂点座標をベクトルに代入
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1ベクトル、p0->p2ベクトルを計算
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//外積（垂直なベクトル）
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//求めた法線を頂点データに代入
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == CIRCLE)*/
		//{
		//	for (int i = 0; i < _countof(indicesCircle) / 3; i++)
		//	{//三角形一つごとに計算
		//		//三角形のインデックスを取り出して、一時的な変数に入れる
		//		unsigned short index0 = indicesCircle[i * 3 + 0];
		//		unsigned short index1 = indicesCircle[i * 3 + 1];
		//		unsigned short index2 = indicesCircle[i * 3 + 2];
		//		//三角形を構成する頂点座標をベクトルに代入
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1ベクトル、p0->p2ベクトルを計算
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//外積（垂直なベクトル）
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//求めた法線を頂点データに代入
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == LINE)*/
		//{
		//	//求めた法線を頂点データに代入
		//	vertices[0].normal = XMFLOAT3(-1.0f / 3.0f, 1.0f / 3.0f, -1.0f / 3.0f);//仮の数字なので後で変更
		//	vertices[1].normal = XMFLOAT3(-1.0f / 3.0f, 1.0f / 3.0f, -1.0f / 3.0f);//〃

		//}
	}
}

void Draw::CreateModel(const char* fileName)
{
	//ファイルストリーム
	std::ifstream file;
	//.objファイルを開く
	file.open(fileName);
	//ファイルオープン失敗をチェック
	assert(file.is_open());

	std::stringstream all;
	//ファイルの内容を文字列ストリームにコピー
	all << file.rdbuf();

	//ファイルを閉じる
	file.close();


	//vertex用
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;

	//1行分の文字列を入れる変数
	std::string line;

	while (std::getline(all,line))
	{
		//一行分の文字列をストリームに変換して解析しやすく
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		//先頭文字列が'v'なら頂点座標
		if (key == "v")
		{
			//x,y,z座標読み込み
			XMFLOAT3 pos{};
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			//座標データに追加
			positions.emplace_back(pos);
		}
		//テクスチャ
		if (key == "vt")
		{
			//UV成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		//法線ベクトル
		if (key == "vn")
		{
			//X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		//ポリゴン
		if (key == "f")
		{
			//半角スペース区切りで行の続きを読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//頂点インデックス一個分の文字列をストリームに変換して解析しやすく
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				//頂点インデックス等に追加(頂点インデックス、法線、uvの順)
				//(1// 2// 3//　←("//")には非対応)
				index_stream.seekg(1, std::ios_base::cur);//スラッシュ(/)を飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexNormal;
		
				

				//頂点データの追加
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				verticesM.emplace_back(vertex);
				//インデックスデータの追加
				indicesM.emplace_back((unsigned short)indicesM.size());
			}
		}
	}

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * verticesM.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indicesM.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuffM));
	assert(SUCCEEDED(Directx::GetInstance().result));


	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	Directx::GetInstance().result = vertBuffM->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {
		std::copy(verticesM.begin(), verticesM.end(), vertMap);
		vertBuffM->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbViewM.BufferLocation = vertBuffM->GetGPUVirtualAddress();
	vbViewM.SizeInBytes = sizeVB;
	vbViewM.StrideInBytes = sizeof(vertices[0]);

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	BuffProperties(heapProps, resourceDesc, &indexBuffM);

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	Directx::GetInstance().result = indexBuffM->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {

		std::copy(indicesM.begin(), indicesM.end(), indexMap);

		indexBuffM->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibViewM.BufferLocation = indexBuffM->GetGPUVirtualAddress();
	ibViewM.Format = DXGI_FORMAT_R16_UINT;
	ibViewM.SizeInBytes = sizeIB;
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
	//ID3D12Resource* texBuff = nullptr;
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
		&textureHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[count]));
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



	//設定をもとにSRV用デスクリプタヒープを生成
	if (count == 0)
	{                                                          //descは設定
		Directx::GetInstance().result = Directx::GetInstance().device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
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
	Directx::GetInstance().device->CreateShaderResourceView(texBuff[count].Get(), &srvDesc, srvHandle);

	int count2 = count;
	count++;

	//04_02(画像貼る用のアドレスを引数に)
	//SRVヒープの設定コマンド
	Directx::GetInstance().commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//SRVヒープのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count2);
}

void Draw::Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
	const bool& primitiveMode)
{
	worldMat->SetWorld();

	//変換行列をGPUに送信
	if (worldMat->parent != nullptr && indexNum != SPRITE)//親がいる場合
	{
		worldMat->matWorld *= worldMat->parent->matWorld;
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}
	else if(indexNum != SPRITE)//親がいない場合
	{
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}

	

	if (indexNum != SPHERE && indexNum != SPRITE)
	{
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
	}
	else//球体の時
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = vertBuff2->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(verticesSphere); i++) {

			vertMap[i] = verticesSphere[i]; // 座標をコピー

		}
		// 繋がりを解除
		vertBuff2->Unmap(0, nullptr);
	}
	
	// ビューポート設定コマンドを、コマンドリストに積む
	Directx::GetInstance().commandList->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // 切り抜き座標下
	// シザー矩形設定コマンドを、コマンドリストに積む
	Directx::GetInstance().commandList->RSSetScissorRects(1, &scissorRect);

	// パイプラインステートとルートシグネチャの設定コマンド
	if (indexNum == LINE)
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineState[2].Get());
	}
	else if (indexNum == SPRITE)
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineSet.pipelineState.Get());
	}
	else
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineState[pipelineNum].Get());
	}

	if (indexNum == SPRITE)
	{
		Directx::GetInstance().commandList.Get()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	}
	else if (indexNum != SPRITE)
	{
		Directx::GetInstance().commandList->SetGraphicsRootSignature(rootSignature.Get());
	}

	if (indexNum != SPHERE)
	{
		if (indexNum == SPRITE)
		{
			// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
			VertexSprite* vertMap = nullptr;
			Directx::GetInstance().result = vertBuffS->Map(0, nullptr, (void**)&vertMap);
			assert(SUCCEEDED(Directx::GetInstance().result));
			// 全頂点に対して
			for (int i = 0; i < 4; i++) {
				vertMap[i] = verticesS[i]; // 座標をコピー
			}
			// 繋がりを解除
			vertBuffS->Unmap(0, nullptr);

			Directx::GetInstance().commandList.Get()->IASetVertexBuffers(0, 1, &vbViewS);


			Directx::GetInstance().commandList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}
		else if (indexNum == MODEL)
		{
			Directx::GetInstance().commandList.Get()->IASetVertexBuffers(0, 1, &vbViewM);
			Directx::GetInstance().commandList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else
		{
			// プリミティブ形状の設定コマンド
			if (indexNum != LINE)
				Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト
			else//線の時（LINE）
				Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 線

			// 頂点バッファビューの設定コマンド
			Directx::GetInstance().commandList->IASetVertexBuffers(0, 1, &vbView);
		}
	}
	else//球体の時
	{
		Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角ストリップ
		// 頂点バッファビューの設定コマンド
		Directx::GetInstance().commandList->IASetVertexBuffers(0, 1, &vbView2);
	}
	

	//定数バッファビュー(CBV)の設定コマンド
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//04_02
	/*if (textureHandle != NULL)*/
	{
		//SRVヒープの設定コマンド
		Directx::GetInstance().commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandle;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		Directx::GetInstance().commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}
	//定数バッファビュー(CBV)の設定コマンド
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

	//インデックスバッファビューの設定コマンド
	if (indexNum != SPRITE)
	{
		if (indexNum == SPHERE)Directx::GetInstance().commandList->IASetIndexBuffer(&ibView3);
		else if (indexNum == MODEL) Directx::GetInstance().commandList->IASetIndexBuffer(&ibViewM);
		else if (indexNum != CIRCLE) Directx::GetInstance().commandList->IASetIndexBuffer(&ibView);
		else if (indexNum == CIRCLE)Directx::GetInstance().commandList->IASetIndexBuffer(&ibView2);
		
	}

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
		break;//ibview（+vbview）を図形ごとに用意する！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	case CIRCLE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesCircle), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case SPHERE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesSphere), 1, 0, 0, 0); // 全ての頂点を使って描画
		break;
	case SPRITE:
		Directx::GetInstance().commandList->DrawInstanced(4, 1, 0, 0); // 全ての頂点を使って描画
		break;
	case MODEL:
		Directx::GetInstance().commandList->DrawIndexedInstanced((UINT)indicesM.size(), 1, 0, 0, 0); // 全ての頂点を使って描画
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

	vertices[0] = { pos1,{vertices[0].normal},{0.0f,1.0f} };//左下
	vertices[1] = { pos2,{vertices[1].normal},{0.5f,0.0f} };//上
	vertices[2] = { pos3,{vertices[2].normal},{1.0f,1.0f} };//右下
	vertices[3] = vertices[1];//右上

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(TRIANGLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0].normal = { 0.0f,0.0f,-1.0f };//左下
	vertices[1].normal = { 0.0f,0.0f,-1.0f };//左上
	vertices[2].normal = { 0.0f,0.0f,-1.0f };//右下
	vertices[3].normal = { 0.0f,0.0f,-1.0f };//右上
	
	vertices[0].pos = {-5.0f,-5.0f,-5.0f};//左下
	vertices[1].pos = {-5.0f,5.0f, -5.0f};//左上
	vertices[2].pos = {5.0f,-5.0f, -5.0f};//右下
	vertices[3].pos = {5.0f,5.0f,  -5.0f};//右上

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(BOX, pipelineNum, textureHandle,cbt);
}

void Draw::DrawBoxSprite(const Vec3& pos, const float& scale,
	XMFLOAT4 color , const UINT64 textureHandle, const Vec2& ancorUV, float rotation, const int& pipelineNum)
{
	//if (widthHeight.x != NULL && widthHeight.y != NULL)
	//{
	//	sprite.vertices[0] = { {-widthHeight.x / 2.0f,+widthHeight.y / 2.0f,0.0f},{0.0f,1.0f} };//左下
	//	sprite.vertices[1] = { {-widthHeight.x / 2.0f,-widthHeight.y / 2.0f,0.0f},{0.0f,0.0f} };//左上
	//	sprite.vertices[2] = { {+widthHeight.x / 2.0f,+widthHeight.y / 2.0f,0.0f},{1.0f,1.0f} };//右下
	//	sprite.vertices[3] = { {+widthHeight.x / 2.0f,-widthHeight.y / 2.0f,0.0f},{1.0f,0.0f} };//右上
	//}
	//else
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	verticesS[0] = { {-(float)(resDesc.Width * scale * ancorUV.x),+(float)(resDesc.Height * scale * (1.0f - ancorUV.y)),0.0f},{0.0f,1.0f} };//左下
	verticesS[1] = { {-(float)(resDesc.Width * scale * ancorUV.x),-(float)(resDesc.Height * scale * (ancorUV.y)),0.0f},{0.0f,0.0f} };//左上
	verticesS[2] = { {+(float)(resDesc.Width * scale * (1.0f - ancorUV.x)),+(float)(resDesc.Height * scale * (1.0f - ancorUV.y)),0.0f},{1.0f,1.0f} };//右下
	verticesS[3] = { {+(float)(resDesc.Width * scale * (1.0f - ancorUV.x)),-(float)(resDesc.Height * scale * (ancorUV.y)),0.0f},{1.0f,0.0f} };//右上
	

	/*if(color.x!=NULL&& color.y != NULL&& color.z != NULL&& color.w != NULL)*/ constMapMaterial->color = color;
	
	//ワールド行列
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);
	worldMat.trans = { pos.x /*+ resDesc.Width * ancorUV.x * scale*/,pos.y/* + resDesc.Height * ancorUV.y * scale*/,0.0f };
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

	cbt.constMapTransform->mat = matW * view.matView * projection.matProjection;

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop, float rotation, const int& pipelineNum)
{
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	float texLeft = UVleftTop.x * +(float)resDesc.Width * scale;
	float texRight = (UVleftTop.x + UVlength.x) * +(float)resDesc.Width * scale;
	float texTop = UVleftTop.y * +(float)resDesc.Height * scale;
	float texBottom = (UVleftTop.y + UVlength.y) * +(float)resDesc.Height * scale;

	if (isPosLeftTop)
	{
		//左上からの座標
		verticesS[0] = { {0,UVlength.y * resDesc.Height * scale,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		verticesS[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		verticesS[2] = { {UVlength.x * resDesc.Width * scale,UVlength.y * resDesc.Height * scale,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		verticesS[3] = { {UVlength.x * resDesc.Width * scale,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	else
	{
		//切り抜いた後の画像の中心からの位置！！！！！！！！
		verticesS[0] = { {-UVlength.x * resDesc.Width * scale / 2.0f,UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//左下
		verticesS[1] = { {-UVlength.x * resDesc.Width * scale / 2.0f,-UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//左上
		verticesS[2] = { {UVlength.x * resDesc.Width * scale / 2.0f,UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//右下
		verticesS[3] = { {UVlength.x * resDesc.Width * scale / 2.0f,-UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//右上
	}
	/*if(color.x!=NULL&& color.y != NULL&& color.z != NULL&& color.w != NULL)*/ constMapMaterial->color = color;
	
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
		worldMat.trans = { leftTop.x + texLeft + UVlength.x * (float)resDesc.Width * scale / 2.0f,
			leftTop.y + texTop + UVlength.y * (float)resDesc.Height * scale / 2.0f,
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

	cbt.constMapTransform->mat = matW * view.matView * projection.matProjection;

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	//手前
	vertices[0] = { {-5.0f,-5.0f,-5.0f},{vertices[0].normal},{0.0f,1.0f} };//左下
	vertices[1] = { {-5.0f,5.0f, -5.0f},{vertices[1].normal},{0.0f,0.0f} };//左上
	vertices[2] = { {5.0f,-5.0f, -5.0f},{vertices[2].normal},{1.0f,1.0f} };//右下
	vertices[3] = { {5.0f,5.0f,  -5.0f},{vertices[3].normal},{1.0f,0.0f} };//右上

	vertices[4] = { {-5.0f,-5.0f,5.0f},{vertices[4].normal},{0.0f,1.0f} };//左下
	vertices[5] = { {-5.0f,5.0f, 5.0f},{vertices[5].normal},{0.0f,0.0f} };//左上
	vertices[6] = { {5.0f,-5.0f, 5.0f},{vertices[6].normal},{1.0f,1.0f} };//右下
	vertices[7] = { {5.0f,5.0f,  5.0f},{vertices[7].normal},{1.0f,0.0f} };//右上
		//上
	vertices[8] = { {5.0f,5.0f,-5.0f},{vertices[8].normal},{0.0f,1.0f} };//左下
	vertices[9] = { {5.0f,5.0f, 5.0f},{vertices[9].normal},{0.0f,0.0f} };//左上
	vertices[10] = { {-5.0f,5.0f, -5.0f},{vertices[10].normal},{1.0f,1.0f} };//右下
	vertices[11] = { {-5.0f,5.0f, 5.0f},{vertices[11].normal},{1.0f,0.0f} };//右上

	vertices[12] = { {5.0f,-5.0f,-5.0f},{vertices[12].normal},{0.0f,1.0f} };//左下
	vertices[13] = { {5.0f,-5.0f, 5.0f},{vertices[13].normal},{0.0f,0.0f} };//左上
	vertices[14] = { {-5.0f,-5.0f, -5.0f},{vertices[14].normal},{1.0f,1.0f} };//右下
	vertices[15] = { {-5.0f,-5.0f, 5.0f},{vertices[15].normal},{1.0f,0.0f} };//右上

	vertices[16] = { {-5.0f,-5.0f,-5.0f},{vertices[16].normal},{0.0f,1.0f} };//左下
	vertices[17] = { {-5.0f,-5.0f, 5.0f},{vertices[17].normal},{0.0f,0.0f} };//左上
	vertices[18] = { {-5.0f,5.0f, -5.0f},{vertices[18].normal},{1.0f,1.0f} };//右下
	vertices[19] = { {-5.0f,5.0f,  5.0f},{vertices[19].normal},{1.0f,0.0f} };//右上

	vertices[20] = { {5.0f,-5.0f,-5.0f},{vertices[20].normal},{0.0f,1.0f} };//左下
	vertices[21] = { {5.0f,-5.0f, 5.0f},{vertices[21].normal},{0.0f,0.0f} };//左上
	vertices[22] = { {5.0f,5.0f, -5.0f},{vertices[22].normal},{1.0f,1.0f} };//右下
	vertices[23] = { {5.0f,5.0f,  5.0f},{vertices[23].normal},{1.0f,0.0f} };//右上;//左下
	
	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(CUBE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawLine(const Vec3& pos1, const Vec3& pos2, WorldMat* world, ViewMat* view, ProjectionMat* projection,const XMFLOAT4& color,
	const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	vertices[0].pos = { pos1.x,pos1.y,pos1.z };
	vertices[1].pos = { pos2.x,pos2.y,pos2.z };
	vertices[0].normal = { 0,0,-1.0f };//normalの処理（ここ以外も）どうにかする！！！！！！！！！！！！！！！！！！！
	vertices[1].normal = { 0,0,-1.0f };

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

void Draw::DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawModel(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(MODEL, pipelineNum, textureHandle, cbt);
}

void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum)
{
	if (indexNum == SPRITE)
	{
		// 頂点シェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpriteVS.hlsl", // シェーダファイル名
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
			L"Resources/shaders/SpritePS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));
	}
	else
	{
		// 頂点シェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicVS.hlsl", // シェーダファイル名
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
			L"Resources/shaders/BasicPS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));
	}

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; 
	}
	else
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング
	
	pipelineDesc.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// 頂点レイアウトの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	}

	// 図形の形状設定
	if (indexNum == LINE)
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	else
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
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Directx::GetInstance().result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(Directx::GetInstance().result));
	Directx::GetInstance().result = Directx::GetInstance().device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));
	assert(SUCCEEDED(Directx::GetInstance().result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = *rootSig;

	//06_01
	//デプスステンシルステート
	pipelineDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if(indexNum == SPRITE)
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//小さければ合格
	else
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

