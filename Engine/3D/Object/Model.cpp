#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>
#include <algorithm>

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string Model::S_BASE_DIRECTORY_ = "Resources/";
uint32_t Model::sDescriptorHandleIncrementSize_ = 0;


void Model::LoadFromOBJInternal(const std::string& folderName, bool smoothing, bool modelType)
{
	const std::string FILE_NAME = folderName + ".obj";
	const std::string DIRECTORY_PATH = S_BASE_DIRECTORY_ + folderName + "/";

	// ファイルストリーム
	std::ifstream file;
	// .objファイルを開く
	file.open(DIRECTORY_PATH + FILE_NAME);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	name_ = folderName;

	// メッシュ生成
	meshes_.emplace_back(std::move(std::make_unique < Mesh>()));
	//所有権をいったん与える
	std::unique_ptr<Mesh> mesh = std::move(meshes_.back());
	int32_t indexCountTex = 0;
	int32_t indexCountNoTex = 0;

	std::vector<XMFLOAT3> positions;	// 頂点座標
	std::vector<XMFLOAT3> normals;	// 法線ベクトル
	std::vector<XMFLOAT2> texcoords;	// テクスチャUV
	// 1行ずつ読み込む
	std::string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		//マテリアル
		if (key == "mtllib")
		{
			// マテリアルのファイル名読み込み
			std::string filename;
			line_stream >> filename;
			// マテリアル読み込み
			LoadMaterial(DIRECTORY_PATH, filename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {

			// カレントメッシュの情報が揃っているなら
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// 頂点法線の平均によるエッジの平滑化
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				//所有権戻す
				meshes_[meshes_.size() - 1] = std::move(mesh);

				// 次のメッシュ生成
				meshes_.emplace_back(std::move(std::make_unique<Mesh>()));
				mesh = std::move(meshes_.back());
				indexCountTex = 0;
			}

			// グループ名読み込み
			std::string groupName;
			line_stream >> groupName;

			// メッシュに名前をセット
			mesh->SetName(groupName);
		}
		// 先頭文字列がvなら頂点座標
		if (key == "v") {
			// X,Y,Z座標読み込み
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		// 先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			// U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V方向反転
			texcoord.y = 1.0f - texcoord.y;
			// テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		// 先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			// X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// 法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr) {
				// マテリアルの名読み込み
				std::string materialName;
				line_stream >> materialName;

				// マテリアル名で検索し、マテリアルを割り当てる
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second.get());
				}
			}
		}
		// 先頭文字列がfならポリゴン（三角形）
		if (key == "f")
		{
			int32_t faceIndexCount = 0;
			// 半角スペース区切りで行の続きを読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// 頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				uint16_t indexPosition, indexNormal, indexTexcoord;
				// 頂点番号
				index_stream >> indexPosition;

				Material* pMaterial = mesh->GetMaterial();
				index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
				// マテリアル、テクスチャがある場合
				if (pMaterial && pMaterial->textureFilename_.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
					index_stream >> indexNormal;
					// 頂点データの追加
					Mesh::VertexPosNormalUvSkin vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					// エッジ平滑化用のデータを追加
					if (smoothing) {
						mesh->AddSmoothData(indexPosition, (uint16_t)mesh->GetVertexCount() - 1);
					}
				}
				else {
					char c;
					index_stream >> c;
					// スラッシュ2連続の場合、頂点番号のみ
					if (c == '/') {
						// 頂点データの追加
						Mesh::VertexPosNormalUvSkin vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
					}
					else {
						index_stream.seekg(-1, std::ios_base::cur); // 1文字戻る
						index_stream >> indexTexcoord;
						index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
						index_stream >> indexNormal;
						// 頂点データの追加
						Mesh::VertexPosNormalUvSkin vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
						// エッジ平滑化用のデータを追加
						if (smoothing) {
							mesh->AddSmoothData(indexPosition, (uint16_t)mesh->GetVertexCount() - 1);
						}
					}
				}
				// インデックスデータの追加
				if (faceIndexCount >= 3) {

					if (modelType == false)
					{
						// 四角形ポリゴンの4点目なので、
						// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
						mesh->AddIndex(indexCountTex - 1);
						mesh->AddIndex(indexCountTex);
						mesh->AddIndex(indexCountTex - 3);
					}
					else
					{
						mesh->PopIndex();
						mesh->PopIndex();
						mesh->PopIndex();

						mesh->AddIndex(indexCountTex - 1);
						mesh->AddIndex(indexCountTex - 2);
						mesh->AddIndex(indexCountTex - 3);

						mesh->AddIndex(indexCountTex - 3);
						mesh->AddIndex(indexCountTex);
						mesh->AddIndex(indexCountTex - 1);
					}
				}
				else
				{
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	// 頂点法線の平均によるエッジの平滑化
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//所有権戻す
	meshes_[meshes_.size() - 1] = std::move(mesh);
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	std::unique_ptr<Material> material;

	// 1行ずつ読み込む
	std::string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin()); // 先頭の文字を削除
		}

		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {

			// 既にマテリアルがあれば
			if (material.get()) {
				// マテリアルをコンテナに登録
				AddMaterial(std::move(material));
			}

			// 新しいマテリアルを生成
			material = Material::Create();
			// マテリアル名読み込み
			line_stream >> material->name_;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient_.x;
			line_stream >> material->ambient_.y;
			line_stream >> material->ambient_.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse_.x;
			line_stream >> material->diffuse_.y;
			line_stream >> material->diffuse_.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular_.x;
			line_stream >> material->specular_.y;
			line_stream >> material->specular_.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャファイル名読み込み
			line_stream >> material->textureFilename_;

			// フルパスからファイル名を取り出す
			size_t pos1;
			pos1 = material->textureFilename_.rfind('\\');
			if (pos1 != std::string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

			pos1 = material->textureFilename_.rfind('/');
			if (pos1 != std::string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

		}
	}
	// ファイルを閉じる
	file.close();

	if (material.get()) {
		// マテリアルを登録（所有権を渡す）
		AddMaterial(std::move(material));
	}
}

void Model::AddMaterial(std::unique_ptr<Material> material)
{
	// コンテナに登録
	materials_.emplace(material->name_, std::move(material));
}

void Model::LoadTextures()
{
	std::string directoryPath = S_BASE_DIRECTORY_ + name_ + "/";

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	for (auto& m : materials_) {
		Material* pMaterial = m.second.get();
		srvGpuHandle.ptr = pMaterial->textureHandle_;

		// テクスチャあり
		if (pMaterial->textureFilename_.size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// マテリアルにテクスチャ読み込み
			pMaterial->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
		}
		// テクスチャなし
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// マテリアルにテクスチャ読み込み
			pMaterial->LoadTexture(S_BASE_DIRECTORY_, cpuDescHandleSRV, gpuDescHandleSRV);
		}
	}
}


std::unique_ptr<Model> Model::LoadFromOBJ(const std::string& folderName, bool smoothing, bool modelType)
{
	//新たなModel型のインスタンスのメモリを確保
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(folderName, smoothing);

	//所有権渡す
	return std::move(model);
}


Model::~Model()
{
	meshes_.clear();
	materials_.clear();
}

void Model::StaticInitialize()
{
	Mesh::StaticInitialize(DirectXWrapper::GetInstance().GetDevice());
}

void Model::Initialize(const std::string& foldername, bool smoothing)
{
	// モデル読み込み
	LoadFromOBJInternal(foldername.c_str(), smoothing);

	// メッシュのマテリアルチェック
	for (auto& m : meshes_) {
		// マテリアルの割り当てがない
		if (m && m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";

				// デフォルトマテリアルをセット(メッシュに渡すのはポインタのみ（所有権は渡さない）)
				m->SetMaterial(defaultMaterial_.get());

				//追加（所有権渡す）
				materials_.emplace(defaultMaterial_->name_, std::move(defaultMaterial_));
			}
			//// デフォルトマテリアルをセット(メッシュに渡すのはポインタのみ（所有権は渡さない）)
			//m->SetMaterial(defaultMaterial_.get());
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes_) {
		if (m) {
			m->CreateBuffers();
		}
	}
	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials_) {
		m.second->Update();
	}

	// テクスチャの読み込み
	LoadTextures();
}

void Model::Draw(uint32_t indexNum)
{
	// デスクリプタヒープの配列
	if (TextureManager::GetInstance().sSrvHeap_) {
		ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetInstance().sSrvHeap_.Get() };
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(indexNum, ppHeaps);
	}

	// 全メッシュを描画
	for (auto& mesh : meshes_) {
		mesh->Draw(DirectXWrapper::GetInstance().GetCommandList());
	}
}