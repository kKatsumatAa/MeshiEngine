/*
* @file Primitive.h
* @brief モデルを使ってない単純な図形
*/

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

public:
	static const int8_t S_BOX_INDICES_NUM_ = 6;
	static const int8_t S_TRIANGLE_INDICES_NUM_ = 3;
	static const int8_t S_CUBE_INDICES_NUM_ = 36;
	static const int8_t S_CIRCLE_INDICES_NUM_ = 66;
	static const int8_t S_LINE_INDICES_NUM_ = 2;
	//球のインデックス用
	static const uint8_t S_SPHERE_INDICES_NUM_ = (66 * 3 + 6);
	static const uint8_t S_SPHERE_WIDTH_ = 36;
	static const uint8_t S_SPHERE_HEIGHT_ = 34;
	//球の同じ列の頂点のペア
	static const uint8_t S_SAME_COLUMN_PAIR_NUM_ = S_SPHERE_WIDTH_ - 1;
	static const uint8_t PAIR_ = 2;

	const float NORMAL_RADIUS_ = 1.0f;

public:
	const float TRIANGLE_ANGLE_1_ = PI * 2.0f + PI * 2.0f / (float)IModel::S_MESH_VERTEX_NORMAL_NUM_ * 2.0f;
	const float TRIANGLE_ANGLE_2_ = PI * 2.0f;
	const float TRIANGLE_ANGLE_3_ = PI * 2.0f + PI * 2.0f / (float)IModel::S_MESH_VERTEX_NORMAL_NUM_;
	const Vec2 TRIANGLE_UV_1_ = { 0.0f,1.0f };
	const Vec2 TRIANGLE_UV_2_ = { 0.5f,0.0f };
	const Vec2 TRIANGLE_UV_3_ = { 1.0f,1.0f };

	static const int8_t S_MESH_VERTEX_NUM_NORMAL_ = 3;
	static const int8_t S_CENTER_FIRST_VERTEX_NUM_ = 2;

private:
	//いろんな図形用
	Vertex verticesTriangle_[3] = {
		//手前
		{{-1.0f,-1.0f,0.0f},{}, {0.0f,1.0f},{}},//左下
		{{-1.0f,1.0f, 0.0f},{},{0.0f,0.0f},{}},//左上
		{{1.0f,-1.0f, 0.0f},{},{1.0f,1.0f},{}},//右下
	};
	Vertex verticesBox_[4] = {
		//手前
		{{-1.0f,-1.0f,0.0f},{},{0.0f,1.0f},{}},//左下
		{{-1.0f,1.0f, 0.0f},{},{0.0f,0.0f},{}},//左上
		{{1.0f,-1.0f, 0.0f},{},{1.0f,1.0f},{}},//右下
		{{1.0f,1.0f,  0.0f},{},{1.0f,0.0f},{}},//右上
	};
	Vertex verticesCircle_[24] = {};
	Vertex verticesCube_[24] = {};
	Vertex verticesLine_[2] = {};
	//球体
	Vertex verticesSphere_[2 + 34 * 36];

private:
	static uint16_t sIndicesBox_[S_BOX_INDICES_NUM_];
	static uint16_t sIndicesTriangle_[S_TRIANGLE_INDICES_NUM_];
	static uint16_t sIndicesCube_[S_CUBE_INDICES_NUM_];
	static uint16_t sIndicesCircle_[S_CIRCLE_INDICES_NUM_];
	static uint16_t sIndicesLine_[S_LINE_INDICES_NUM_];
	static uint16_t sIndicesSphere_[Primitive::S_SPHERE_INDICES_NUM_ * S_SPHERE_WIDTH_];

	static uint32_t sizeVB_;


public:
	//初期化
	void Initialize();

	//三角形初期化
	void InitializeTriangle();
	//四角形初期化
	void InitializeBox();
	//円初期化
	void InitializeCircle();
	//立方体初期化
	void InitializeCube();
	//線初期化
	void InitializeLine();
	//球初期化
	void InitializeSphere();

	//Drawに必要なコマンド
	void DrawCommandPrimitive(ID3D12Resource* vertBuff, int32_t vertexCount, Vertex* vertex, const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
		const D3D12_VERTEX_BUFFER_VIEW& buffView, const D3D12_INDEX_BUFFER_VIEW& ibView, int32_t indicesCount,
		const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);

public:
	//三角形描画
	void TriangleDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	//四角形描画
	void BoxDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	//円描画
	void CircleDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	//立方体描画
	void CubeDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	//線描画
	void LineDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	//球描画
	void SphereDraw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
};
