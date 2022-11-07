#pragma once
#include "Sprite.h"


//リソース設定
extern D3D12_RESOURCE_DESC resDesc;
//設定をもとにSRV用デスクリプタヒープを生成
extern ComPtr < ID3D12DescriptorHeap> srvHeap;
extern D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

extern ComPtr<ID3D12Resource> texBuff[512];



//SRVの最大個数
const size_t kMaxSRVCount = 2056;
//デスクリプタヒープの設定
static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {
srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
srvHeapDesc.NumDescriptors = kMaxSRVCount,
srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE//シェーダーから見えるように
};

struct Vertex
{
	XMFLOAT3 pos;   //xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;    //uv座標
};
//頂点データ

class Model
{
private:

	//マテリアル
	struct Material
	{
		std::string name;//マテリアル名
		XMFLOAT3 ambient;//アンビエント影響度
		XMFLOAT3 diffuse;//ディフューズ〃
		XMFLOAT3 specular;//スペキュラー〃
		float alpha;//アルファ
		std::string textureFilename;//テクスチャファイル名
		UINT64 textureHandle;

		//コンストラクタ
		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

public:
	//モデル用
//頂点データ配列
	std::vector<Vertex> verticesM;
	//頂点インデックス配列
	std::vector<unsigned short> indicesM;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuffM;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffM;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbViewM{};
	D3D12_INDEX_BUFFER_VIEW ibViewM{};

	//マテリアル
	Material material;


public:
	//"フォルダ名のみ"を指定すればmtl,obj,textuerを読みこむ（すべて同じ名前であれば）
	void CreateModel(const char* folderName);
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

};




void LoadGraph(const wchar_t* name, UINT64& textureHandle);