#pragma once

#include "DirectX.h"


class Draw
{
private:
	// パイプランステートの生成
	ID3D12PipelineState* pipelineState[2] = { nullptr };
	// ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//04_01
		//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;//xyz座標
		XMFLOAT2 uv;//uv座標
	};
	//頂点データ
	Vertex vertices[4] = {
		{{-50.0f,-50.0f,0.0f},{0.0f,1.0f}},//左下
		{{-50.0f,50.0f, 0.0f},{0.0f,0.0f}},//左上
		{{50.0f,-50.0f, 0.0f},{1.0f,1.0f}},//右下
		{{50.0f,50.0f,  0.0f},{1.0f,0.0f}},//右上
	};
	unsigned short indices[6] =
	{
		0,1,2,//三角形1つ目
		1,2,3,//三角形2つ目
	};
	unsigned short indices2[3] =
	{
		0,1,2//三角形2つ目
	};
	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[2] = {
	{//xyz座標
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//uv座標
	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)
	};
	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファの生成
	ID3D12Resource* constBuffMaterial = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色(RGBA)
	};
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	XMFLOAT4 color2 = { 0,0,0,0 };
	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//定数バッファ用データ構造
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D変換行列
	};
	//05_02
	ID3D12Resource* constBuffTransform = nullptr;//定数バッファのGPUリソースのポインタ
	KeyboardInput key;

	// 2.描画先の変更
	// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	Directx directx;
	WindowsApp win;
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;
	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	srvHeapDesc.NumDescriptors = kMaxSRVCount,
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE//シェーダーから見えるように
	};
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};
	//頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;


	//--------------------
	void Update( unsigned short* indices,  const int& pipelineNum,
		const bool& primitiveMode= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
	ConstBufferDataTransform* constMapTransform = nullptr;//定数バッファのマッピング用ポインタ

	//
	Draw(const WindowsApp& win, Directx& directx);

	void DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, const int& pipelineNum=0);
	void DrawBox(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, const int& pipelineNum=0);
	void DrawBoxSprite(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, const int& pipelineNum = 0);
	void LoadGraph(const wchar_t* name);

	void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState);

	void Blend(const D3D12_BLEND_OP& blendMode,
		const bool& Inversion = 0, const bool& Translucent = 0);

	void constBuffTransfer(const XMFLOAT4& plusRGBA);

	void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

	void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
		ID3D12Resource** buff);

	void SetNormDigitalMat(XMMATRIX& mat, const WindowsApp& win);

	void Error(const bool& filed);
};