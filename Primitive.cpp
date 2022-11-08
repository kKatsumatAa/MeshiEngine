#include "Primitive.h"
#include "Util.h"



UINT sizeVB;

void Primitive::Initialize()
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
		Vec3 vec = { 0,-5.0f,0 };

		//頂点二つ
		verticesSphere[0] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };//下
		verticesSphere[1] = { {vec.x,-vec.y,vec.z},{},{1.0f,0.0f} };//上

		for (int i = 0; i < 36; i++)//横
		{
			worldMat.rot.y = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int j = 0; j < 34; j++)//縦
			{
				worldMat.rot.x = ((float)(j + 1) * (pi / 35.0f));
				worldMat.SetWorld();
				vec = { 0,-5.0f,0 };
				Vec3xM4(vec, worldMat.matWorld, false);

				int p = i * 34 + j;
				verticesSphere[(2) + i * 34 + j] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };
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

	{
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
}
