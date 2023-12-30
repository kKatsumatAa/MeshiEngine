/*
* @file FbxLoader.h
* @brief fbxのモデルを読み込む
*/

#pragma once
#include "fbxsdk.h"
#include "DirectXWrapper.h"
#include <string>
#include "ModelFBX.h"


class FbxLoader final
{
private://エイリアス
	//std::を省略
	using string = std::string;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//ボーン番号とスキンウェイとのペア
	struct WeightSet
	{
		uint32_t index;
		float weight;
	};


public:
	const int8_t DEFAULT_P_FILE_FORMAT_ = -1;
	const float NODE_TRANS_W_ = 1.0f;
	const float BONE_WEIGHT_RATIO_MAX_ = 1.0f;
	const int8_t ANIMATION_P_SECOND_ = 1;
	//マテリアル
	const XMFLOAT3 DEFAULT_SPECULAR_ = { 0.3f,0.3f,0.3f };
	const float DEFAULT_ALPHA_ = 1.0f;
	//行
	static const int8_t S_MAT_WIDTH_ = 4;
	static const int8_t S_MAT_HEIGHT_ = 4;

private://変数
	//fbxマネージャー(sdkを使うのに必要)
	FbxManager* fbxManager_ = nullptr;
	//fbxインポーター(.fbxを読み込むのに必要)
	FbxImporter* fbxImporter_ = nullptr;

	//スムージング処理するか
	bool isSmoothing_ = false;

	//コントロールポイント用配列
	struct MyControlPoint
	{
		DirectX::XMFLOAT3 pos;
		uint32_t boneIndex[Mesh::S_MAX_BONE_INDICES_] = { 0 };//影響を受けるボーン　番号
		float boneWeight[Mesh::S_MAX_BONE_INDICES_] = { 1.0f,0,0,0 };//ボーン　重み
	};
	std::vector<MyControlPoint> myControlPoints_;

public://定数
	//モデル格納ルートパス
	static const string S_BASE_DIRECTORY_;


private://関数
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader();
public:
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

public:
	/// <summary>
	/// ファイルからfbxモデル読み込み
	/// </summary>
	/// <param name="modelName"></param>
	std::unique_ptr<ModelFBX> LoadModelFromFile(const string& modelName, bool smoothing = false);

	/// <summary>
/// FBXの行列をXMMATRIXに変更
/// </summary>
/// <param name="dst">書き込み先</param>
/// <param name="src">元となるFBX行列</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

	//モデルに別fbxのアニメーションを追加
	void AddAnimationModel(ModelFBX* addToModel, const std::string& animeFbxName);

private:
	//フルパスをゲット
	std::string GetFullPath(const std::string& name);

	//fbxシーンにインポート
	void FbxSceneImport(const std::string& fbxFullPath, FbxScene* fbxScene);

	//グローバルトランスフォーム計算
	void CalcGlobalTransform(const FbxNode& fbxNode, Node& node, Node* parent, float scaleExtend = 1.0f);

	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(ModelFBX* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(ModelFBX* model, FbxNode* fbxNode, Mesh* mesh);

	//頂点座標読み取り
	void ParseMeshControlPointsPos(FbxMesh* fbxMesh);

	//面情報読み取り
	void ParseMeshFaces(FbxMesh* fbxMesh, Mesh* mesh);

	//マテリアル読み取り
	void ParseMaterial(ModelFBX* model, Mesh* mesh, FbxNode* fbxNode);

	//スキニング情報の読み取り
	void PerseSkin(ModelFBX* model, FbxMesh* fbxMesh);

	//アニメーション読み込み
	void LoadAnimation(ModelFBX* model, FbxScene* fbxScene);
};