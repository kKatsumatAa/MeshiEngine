#pragma once
#include "Sprite.h"
#include "Mesh.h"
#include <unordered_map>


struct Vertex
{
	XMFLOAT3 pos;   //xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;    //uv座標
};
//頂点データ


class Model
{
private:
	static const std::string baseDirectory;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;

private:
	// 名前
	std::string name;
	// メッシュコンテナ
	std::vector<Mesh*> meshes;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials;
	// デフォルトマテリアル
	Material* defaultMaterial = nullptr;


private:
	//OBJファイルから3Dモデルを読み込む
	void LoadFromOBJInternal(const std::string& folderName, const bool smoothing = false, const bool& modelType = false);

	/// <summary>
	/// マテリアル登録
	/// </summary>
	void AddMaterial(Material* material);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTextures();

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	//"フォルダ名のみ"を指定すればmtl,obj,textuerを読みこむ（すべて同じ名前であれば）
	static Model* LoadFromOBJ(const std::string& folderName, const bool smoothing = false, const bool& modelType = false);

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	static void StaticInitialize();


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelname">モデル名</param>
	/// <param name="modelname">エッジ平滑化フラグ</param>
	void Initialize(const std::string& foldername, bool smoothing);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList">命令発行先コマンドリスト</param>
	void Draw(const int indexNum = 1);
};