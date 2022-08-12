#pragma once

#include "Util.h"
#include "Sprite.h"


/// <summary>
/// 頂点インデックス用
/// </summary>
enum indices
{
	TRIANGLE,
	BOX,
	CUBE,
	LINE,
	CIRCLE,
	SPHERE,
	SPRITE
};
struct Vertex
{
	XMFLOAT3 pos;   //xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;    //uv座標
};
//頂点データ

class Draw
{
private:
	//リソース設定
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt;//ここをどうにかすれば、インスタンス一つでも色々描画
	
	//頂点バッファの生成
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//04_01
		//頂点データ構造体
	
	
		//定数バッファの生成
	ComPtr < ID3D12Resource> constBuffMaterial = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	
	
	//04_02
	
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle2;
	int count2=0;
	
	////sprite用
	//頂点バッファの生成
	ComPtr < ID3D12Resource> vertBuffS = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbViewS{};
	

private:
	//--------------------
	void Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
		const bool& primitiveMode= true);

public://変数
	WorldMat* worldMat = new WorldMat();
	ViewMat* view = new ViewMat();
	ProjectionMat* projection = new ProjectionMat();
	bool isWireFrame = 0;

public:
	

	//
	Draw();

	void DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,
		WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color={NULL,NULL,NULL,NULL},
		const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		XMFLOAT4 color = { NULL,NULL,NULL,NULL },
		const UINT64 textureHandle = NULL, const int& pipelineNum=0);
	void DrawBoxSprite(const Vec3& pos, const float& scale, XMFLOAT4 color = {NULL,NULL,NULL,NULL}
		, const UINT64 textureHandle = NULL,const Vec2& ancorUV={0,0}, float rotation = 0.0f, const int& pipelineNum = 0);
	void DrawClippingBoxSprite(const Vec3& leftTop, const float& scale,const XMFLOAT2& UVleftTop,const XMFLOAT2& UVlength,
		XMFLOAT4 color = {NULL,NULL,NULL,NULL}, float rotation = 0.0f, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawLine(XMFLOAT3 pos1, XMFLOAT3 pos2, WorldMat* world, ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color
	, const UINT64 textureHandle=NULL, const int& pipelineNum=0);
	void DrawCircle(float radius, WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	
private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};

void LoadGraph(const wchar_t* name, UINT64& textureHandle);

void DrawInitialize();


//
void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	const bool& Inversion = 0, const bool& Translucent = 0);





void SetNormDigitalMat(XMMATRIX& mat);

void Error(const bool& filed);