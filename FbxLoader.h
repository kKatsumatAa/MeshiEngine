#pragma once

#include "fbxsdk.h"
#include "DirectX.h"
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
	void LoadModelFromFile(const string& modelName);

	/// <summary>
	/// 再帰的にノード攻勢を解析
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(ModelFBX* model, FbxNode* fbxNode, Node* parent = nullptr);
};