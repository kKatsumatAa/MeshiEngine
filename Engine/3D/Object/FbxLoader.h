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
	std::vector<MyControlPoint> myControlPoints;

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

private:
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
	void ParseMeshControlPointsPos(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh);

	//面情報読み取り
	void ParseMeshFaces(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh);

	//メッシュの接線取得
	void CalcMeshTangent(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh);

	//マテリアル読み取り
	void ParseMaterial(ModelFBX* model, Mesh* mesh, FbxNode* fbxNode);

	//スキニング情報の読み取り
	void PerseSkin(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh);
};