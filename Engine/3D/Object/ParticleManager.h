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
#include "Camera.h"
#include "RootPipe.h"

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
		Vec3 pos; // xyz座標
		float scale; // スケール
		XMFLOAT4 color;//色
		Vec3 rot;
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	// ビルボード行列
	};

	//ブレンドモード
	enum BLEND_NUM
	{
		ADD,
		SUB,
		TRIANGLE
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
		Vec3 position_ = {};
		// 速度
		Vec3 velocity_ = {};
		// 加速度
		Vec3 accel_ = {};
		// 色
		XMFLOAT4 color_ = {};
		// スケール
		float scale_ = 1.0f;
		// 回転
		Vec3 rotation_ = { 0,0,0 };
		// 初期値
		XMFLOAT4 sColor_ = {};
		float sScale_ = 1.0f;
		Vec3 sRotation_ = { 0,0,0 };
		// 最終値
		XMFLOAT4 eColor_ = {};
		float eScale_ = 0.0f;
		Vec3 eRotation_ = { 0,0,0 };
		// 現在フレーム
		float frame_ = 0;
		// 終了フレーム
		int32_t numFrame_ = 0;
	};

private: // 定数
	static const int32_t S_VERTEX_COUNT_ = 65536;		// 頂点数

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

public: // メンバ関数	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update(float speed, Camera* camera = nullptr);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(uint64_t texHandle = NULL);

	/// <summary>
	/// パーティクルの追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	void Add(int32_t life, const Vec3& position, const Vec3& velocity, const Vec3& accel, float start_scale, float end_scale
		, const XMFLOAT4& start_color = { 1.0f,1.0f,1.0f,1.0f }, const XMFLOAT4& end_color = { 1.0f,1.0f,1.0f,1.0f }, float start_rot = PI * 2.0f, float end_rot = -PI * 2.0f);

	//ランダムに生成
	void GenerateRandomParticle(int32_t num, int32_t lifeTime, float vecPower, Vec3 position, float start_scale, float end_scale
		, const XMFLOAT4& start_color = { 1.0f,1.0f,1.0f,1.0f }, const XMFLOAT4& end_color = { 1.0f,1.0f,1.0f,1.0f });

	void ClearParticles() { particles_.clear(); }

	void SetBlendNum(BLEND_NUM blendNum) { blendNum_ = blendNum; }

private: // メンバ変数
	BLEND_NUM blendNum_ = ADD;
	//ルートシグネチャ等
	RootPipe rootPipe[BLEND_NUM::TRIANGLE + 1];
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	// パーティクル配列
	std::forward_list<Particle> particles_;
	//
	ViewMat* view_ = nullptr;
	ProjectionMat* projection_ = nullptr;

public:
	uint64_t texHandle_ = NULL;


private:
	ParticleManager() { ; }
	~ParticleManager();

public:
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
};

