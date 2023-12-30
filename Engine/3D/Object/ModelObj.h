/*
* @file ModelObj.h
* @brief .objのモデル
*/

#pragma once
#include"IModel.h"


struct Vertex
{
	DirectX::XMFLOAT3 pos;   //xyz座標
	DirectX::XMFLOAT3 normal;//法線ベクトル
	DirectX::XMFLOAT2 uv;    //uv座標
	DirectX::XMFLOAT4 tangent;//法線の接線
};
//頂点データ


class ModelObj : public IModel
{
private:
	//参照先のパス一部（objかfbxか）
	static const std::string S_TYPE_DIRECTORY_;
public:
	const float VERTEX_Z_CHANGE_ = -1.0f;
	const float TEXCOORD_Y_MAX_ = 1.0f;
	const float DEFAULT_BONE_WEIGHT_ = 1.0f;


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
	//マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	//"フォルダ名のみ"を指定すればmtl,obj,textuerを読みこむ（すべて同じ名前であれば）
	static std::unique_ptr<ModelObj> LoadFromOBJ(const std::string& folderName, bool smoothing = false);
};


