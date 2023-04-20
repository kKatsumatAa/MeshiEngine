﻿#pragma once

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
	FbxManager* fbxManager = nullptr;
	//fbxインポーター(.fbxを読み込むのに必要)
	FbxImporter* fbxImporter = nullptr;

	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTexFileName;

public://定数
	//モデル格納ルートパス
	static const string baseDirectory;


private://関数
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

public:
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

	/// <summary>
	/// ファイルからfbxモデル読み込み
	/// </summary>
	/// <param name="modelName"></param>
	ModelFBX* LoadModelFromFile(const string& modelName);

	/// <summary>
	/// 再帰的にノード攻勢を解析
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(ModelFBX* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(ModelFBX* model, FbxNode* fbxNode);

	//頂点座標読み取り
	void ParseMeshVertices(ModelFBX* model, FbxMesh* fbxMesh);

	//面情報読み取り
	void ParseMeshFaces(ModelFBX* model, FbxMesh* fbxMesh);

	//マテリアル読み取り
	void ParseMaterial(ModelFBX* model, FbxNode* fbxNode);

	//テクスチャ読み込み
	void LoadTexture(ModelFBX* model, const std::string& fullpath);

	//ディレクトリを含んだファイルパスからファイル名(***.pmg)を抽出する
	std::string ExtractFileName(const std::string& path);

	/// <summary>
	/// FBXの行列をXMMATRIXに変更
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">元となるFBX行列</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

	//スキニング情報の読み取り
	void PerseSkin(ModelFBX* model, FbxMesh* fbxMesh);
};