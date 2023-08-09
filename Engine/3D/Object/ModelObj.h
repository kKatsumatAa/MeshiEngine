#pragma once
#include"IModel.h"


struct Vertex
{
	DirectX::XMFLOAT3 pos;   //xyz座標
	DirectX::XMFLOAT3 normal;//法線ベクトル
	DirectX::XMFLOAT4 tangent;//法線の接線
	DirectX::XMFLOAT2 uv;    //uv座標
};
//頂点データ


class ModelObj : public IModel
{
private:
	//参照先のパス一部（objかfbxか）
	static const std::string S_TYPE_DIRECTORY_;


private:
	//OBJファイルから3Dモデルを読み込む
	void LoadFromOBJInternal(const std::string& folderName, bool smoothing = false);


	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTextures() override;

	//初期化(継承の関係で)
	void InitializeInternal(const std::string& folderName, bool smoothing);

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	//"フォルダ名のみ"を指定すればmtl,obj,textuerを読みこむ（すべて同じ名前であれば）
	static std::unique_ptr<ModelObj> LoadFromOBJ(const std::string& folderName, bool smoothing = false);
};


