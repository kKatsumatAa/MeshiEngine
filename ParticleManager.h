#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "TextureManager.h"

/// <summary>
/// パーティクルマネージャ
/// </summary>
class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz座標
		float scale; // スケール
		XMFLOAT4 color;//色
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	// ビルボード行列
	};

	// パーティクル1粒
	class Particle
	{
		// Microsoft::WRL::を省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::を省略
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		// 座標
		XMFLOAT3 position = {};
		// 速度
		XMFLOAT3 velocity = {};
		// 加速度
		XMFLOAT3 accel = {};
		// 色
		XMFLOAT4 color = {};
		// スケール
		float scale = 1.0f;
		// 回転
		float rotation = 0.0f;
		// 初期値
		XMFLOAT4 s_color = {};
		float s_scale = 1.0f;
		float s_rotation = 0.0f;
		// 最終値
		XMFLOAT4 e_color = {};
		float e_scale = 0.0f;
		float e_rotation = 0.0f;
		// 現在フレーム
		int frame = 0;
		// 終了フレーム
		int num_frame = 0;
	};

private: // 定数
	static const int vertexCount = 65536;		// 頂点数

public:// 静的メンバ関数
	static ParticleManager* GetInstance();

private:
	/// <summary>
/// グラフィックパイプライン生成
/// </summary>
/// <returns>成否</returns>
	void InitializeGraphicsPipeline();

	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();

	void UpdateMatrix(ViewMat* view, ProjectionMat* projection);

public: // メンバ関数	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update(ViewMat* view, ProjectionMat* projection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(UINT64 texHandle);

	/// <summary>
	/// パーティクルの追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale
		, XMFLOAT4 start_color = { 1.0f,1.0f,1.0f,1.0f }, XMFLOAT4 end_color = { 1.0f,1.0f,1.0f,1.0f }, float start_rot = 0.0f, float end_rot = 0.0f);

	//ランダムに生成
	void GenerateRandomParticle(int num, int lifeTime, float vecPower, Vec3 position, float start_scale, float end_scale
		, XMFLOAT4 start_color = { 1.0f,1.0f,1.0f,1.0f }, XMFLOAT4 end_color = { 1.0f,1.0f,1.0f,1.0f });

private: // メンバ変数
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelinestate;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// パーティクル配列
	std::forward_list<Particle> particles;
	//
	ViewMat* view = nullptr;
	ProjectionMat* projection = nullptr;

	//ビルボード行列
	static XMMATRIX matBillboard;
	static XMMATRIX matBillboardY;
	// ビュー行列
	XMMATRIX matView = DirectX::XMMatrixIdentity();

public:
	UINT64 texHandle = NULL;


private:
	ParticleManager() { ; }
	ParticleManager(const ParticleManager&) = delete;
	~ParticleManager();
	ParticleManager& operator=(const ParticleManager&) = delete;
};

