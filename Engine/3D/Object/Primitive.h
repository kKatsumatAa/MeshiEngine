#pragma once
#include"ModelObj.h"


/// <summary>
/// 図形用の情報を持ったクラス（drawで使うので一個のみ生成）
/// </summary>
class Primitive
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//球のインデックス用
	static const int32_t S_SPHERE_INDEX_NUM_ = 66 * 3 + 6;

	//頂点バッファの生成
	ComPtr < ID3D12Resource> vertBuffTriangle_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffBox_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffCircle_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffCube_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffLine_ = nullptr;
	ComPtr<ID3D12Resource> vertBuffSphere_ = nullptr;

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbTriangleView_{};
	D3D12_VERTEX_BUFFER_VIEW vbBoxView_{};
	D3D12_VERTEX_BUFFER_VIEW vbCircleView_{};
	D3D12_VERTEX_BUFFER_VIEW vbCubeView_{};
	D3D12_VERTEX_BUFFER_VIEW vbLineView_{};
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbViewSphere_{};

	//04_01
		//頂点データ構造体

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp_{};

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibViewTriangle_{};
	D3D12_INDEX_BUFFER_VIEW ibViewBox_{};
	D3D12_INDEX_BUFFER_VIEW ibViewCircle_{};
	D3D12_INDEX_BUFFER_VIEW ibViewLine_{};
	D3D12_INDEX_BUFFER_VIEW ibViewCube_{};
	D3D12_INDEX_BUFFER_VIEW ibViewSphere_{};

	//いろんな図形用
	Vertex verticesTriangle_[3] = {
		//手前
		{{-1.0f,-1.0f,0.0f},{},{}, {0.0f,1.0f}},//左下
		{{-1.0f,1.0f, 0.0f},{},{},{0.0f,0.0f}},//左上
		{{1.0f,-1.0f, 0.0f},{},{},{1.0f,1.0f}},//右下
	};
	Vertex verticesBox_[4] = {
		//手前
		{{-1.0f,-1.0f,0.0f},{},{},{0.0f,1.0f}},//左下
		{{-1.0f,1.0f, 0.0f},{},{},{0.0f,0.0f}},//左上
		{{1.0f,-1.0f, 0.0f},{},{},{1.0f,1.0f}},//右下
		{{1.0f,1.0f,  0.0f},{},{},{1.0f,0.0f}},//右上
	};
	Vertex verticesCircle_[24] = {};
	Vertex verticesCube_[24] = {};
	Vertex verticesLine_[2] = {};
	//球体
	Vertex verticesSphere_[2 + 34 * 36];

private:
	static uint16_t sIndicesBox_[6];
	static uint16_t sIndicesTriangle_[3];
	static uint16_t sIndicesCube_[36];
	static uint16_t sIndicesCircle_[66];
	static uint16_t sIndicesLine_[2];
	static uint16_t sIndicesSphere_[Primitive::S_SPHERE_INDEX_NUM_ * 36];

	static uint32_t sizeVB_;


public:
	//初期化系
	void Initialize();

	void InitializeTriangle();
	void InitializeBox();
	void InitializeCircle();
	void InitializeCube();
	void InitializeLine();
	void InitializeSphere();

	//Drawに必要なコマンド
	void DrawCommandPrimitive(ID3D12Resource* vertBuff, int32_t vertexCount, Vertex* vertex, const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
		const D3D12_VERTEX_BUFFER_VIEW& buffView, const D3D12_INDEX_BUFFER_VIEW& ibView, int32_t indicesCount,
		const std::function<void()>& setRootParam,const std::function<void()>& setMaterialLightTex);

	void TriangleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void BoxDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void CircleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void CubeDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void LineDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void SphereDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
};
